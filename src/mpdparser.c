#include "mpdparser.h"
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

void mpdparser_get_xml_prop_type(xmlNode *a_node, const char *property_name, int *property_value)
{
    xmlChar *prop_str;

    *property_value = MPD_TYPE_LIVE;   
    prop_str = xmlGetProp(a_node, (const xmlChar*)property_name);
    if (prop_str)
    {
        if (xmlStrcmp(prop_str, (xmlChar *) "OnDemand") == 0
            || xmlStrcmp(prop_str, (xmlChar *) "static"))
        {
            LOG(INFO, "MPD static"); 
        }
        else if (xmlStrcmp(prop_str, (xmlChar *) "Live") == 0
            || xmlStrcmp(prop_str, (xmlChar *) "dynamic"))
        {
            LOG(FATAL, "MPD dynamic, not support yet"); 
        } 
        else
        {
            LOG(FATAL, "failed to parser MPD type");
        }

       xmlFree(prop_str);
    }
    else
    {
        LOG(FATAL, "MPD file without type");
    }
    return;
}


void mpdparser_get_xml_prop_duration(xmlNode *a_node, const char *property_name, long long default_value, long long *property_value)
{
    xmlchar *prop_str;
    char *str;

    *property_value = default_value;
    prop_str = xmlGetProp(a_node, (const xmlChar *)property_name);
    if (prop_str)
    {
        str = (char *)prop_str;
        if (mpdparser_parse_duration(str, property_value))
        {
            xmlFree(prop_str);
            return;
        }
        LOG(INFO, "%s:%lld", property_name, *property_value);
        xmlFree(prop_str);
        return;
    }
}



int mpdparser_parse_root_node(struct MPDNode **ptr, xmlNode *a_node, char *mpdURL)
{
    xmlNode *cur_node;
    struct MPDNode *new_mpd;

    new_mpd =(struct MPDNode *) malloc(sizeof(struct MPDNode));
    if (!new_mpd)
    {
        LOG(FATAL, "malloc failed"); 
        return MPD_PARSE_ERROR; 
    }

    memset(new_mpd, 0, sizeof(struct MPDNode));

    mpdparser_get_xml_prop_type(a_node, "type", &new_mpd->type);
    if (new_mpd->type == MPD_TYPE_LIVE)
    {
        goto error;
    }

    mpdparser_get_xml_prop_duration(a_node, "mediaPresentationDuration", 0, &new_mpd->mediaPresentationDuration);


   for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
   {
       if (cur_node->type == XML_ELEMENT_NODE) 
       {
           if (xmlStrcmp(cur_node->name, (xmlChar *)"BaseURL") == 0 && !(new_mpd->BaseURL))       
           {
               if(mpdparser_parse_baseURL_node(&new_mpd->BaseURL, &new_mpd->PathURI, mpdURL, NULL, cur_node))
               {
                   goto error; 
               }
           }
           else if (xmlStrcmp(cur_node->name, (xmlChar *)"Period") == 0)
           {
               if (mpdparser_parse_period_node(&new_mpd->Periods, cur_node))
               {
                   goto error;
               }
           }
       
       }
   
   }

   *ptr = new_mpd;
   return MPD_PARSE_OK;

error:
   mpdparser_free_mpd_node(new_mpd);
   return MPD_PARSE_ERROR;
}



