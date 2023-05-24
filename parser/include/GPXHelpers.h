/**
 * Name: Mahimna Pyakuryal (mpyakury)
 * Student ID: 1014826
 * Used code from following examples provided by professor:
 *      libXmlExample.c 
 *      StructListDemo.c
 * For A2 I used the following links provided by the professor:
 *      -> http://www.xmlsoft.org/examples/tree2.c
 *      -> http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
 *      -> https://www.movable-type.co.uk/scripts/latlong.html
 */

#ifndef GPX_HELPERS_H
#define GPX_HELPERS_H

#define MAX_LONLAT_LENGTH 12 //needed to string wpt attributes lon/lat

void wptHelper(xmlNode *a_node, List *builder, char choice[]);
void rteHelper (xmlNode *a_node, List *builder);
void trkHelper (xmlNode *a_node, List *builder); 

void childCheckWPT (xmlNode *a_node, Waypoint *wptNode);
void childCheckRTE (xmlNode *a_node, Route *rteNode);
void childCheckTRK (xmlNode *a_node, Track *trkNode);

void rteptHelper (xmlNode *a_node, Waypoint *tmpWPT); 

void attrCheck (xmlNode *a_node, Waypoint *wptNode);

Waypoint* createWPT (); 
Route* createRTE();
Track* createTRK();
TrackSegment* createSeg();

float haversine (float firstLat, float firstLon, float secondLat, float secondLon); 

/**
 * takes doc, coverts to tree, writes tree to file
 */
bool writeHelper (GPXdoc *toWrite, char *fileName); 
bool validateTreeHelper (xmlDocPtr doc, char *gpxSchemaFile); //will free tree passed in 
xmlDocPtr createTree (GPXdoc *toWrite); 
bool writeOutHelper (xmlDocPtr doc, char *fileName);
bool docValidity(GPXdoc *doc); 

void dummyDelete (void *data); 
#endif 