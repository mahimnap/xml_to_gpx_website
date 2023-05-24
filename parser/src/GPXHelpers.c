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

#include "GPXParser.h"
#include "GPXHelpers.h"

/**
 * following are some 'constructor' methods for 
 * all the nodes in our list except GPXDoc 
 * - this is done in the public function itself
 */
Track* createTRK(){

    Track *returnTRK = (Track *) malloc (sizeof (Track)); 
    returnTRK->name = (char *) malloc (1); 
    strcpy (returnTRK->name, ""); 
    returnTRK->segments = initializeList (&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
    returnTRK->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    return returnTRK; 
}

TrackSegment* createSeg() {

    TrackSegment *returnSeg = (TrackSegment *) malloc (sizeof (TrackSegment)); 
    returnSeg->waypoints = initializeList (&waypointToString, &deleteWaypoint, &compareWaypoints);
    return returnSeg; 
}

Route* createRTE(){

    Route *returnRTE = (Route *) malloc (sizeof (Route)); 
    returnRTE->name = (char *) malloc (1); 
    strcpy (returnRTE->name, ""); 
    returnRTE->waypoints = initializeList (&waypointToString, &deleteWaypoint, &compareWaypoints);
    returnRTE->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    return returnRTE; 
}

/**
 * used for trkpt/rtept too
 * i.e. all wpt types
 */
Waypoint* createWPT (){

    Waypoint *returnWPT = (Waypoint *) malloc (sizeof (Waypoint));
    returnWPT->name = (char *) malloc (1);
    strcpy (returnWPT->name, "");
    returnWPT->longitude = 0; 
    returnWPT->latitude = 0; 
    returnWPT->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    return returnWPT; 
}

/**
 * Receives a node -> at <gpx> (root) level if called from createDoc, at <trkseg> level if called from childCheckTrk
 * for loop goes into children of passed in node 
 * checks to see if choice is found (trkpt or wpt) 
 * if found, create a new waypoint, check for attributes in waypoint (lon/lat), check for children of waypoint (name/other)
 * then insert back into list provided (either doc->waypoints or tracksegment->waypoints)
 */

void wptHelper (xmlNode *a_node, List *builder, char choice[]){
    xmlNode *cur_node = NULL;
    Waypoint *tmp; 
    for (cur_node = a_node->children; cur_node != NULL; cur_node = cur_node->next){
        if (cur_node->type == XML_ELEMENT_NODE) { 
            if (strcmp ((char *)cur_node->name, choice) == 0){
                tmp = createWPT();
                attrCheck (cur_node, tmp);
                childCheckWPT (cur_node, tmp);
                insertBack (builder, tmp);
            }
        }
    }
}

/**
 * checks for children of a waypoint as described above 
 * for loop goes into child of wpt or trkpt 
 * possible children of wpt/trkpt = name or other data 
 * other data is a flexible array so mallocing size of name of otherdata will malloc 
 * space for the string as well as the name
 */
void childCheckWPT (xmlNode *a_node, Waypoint *wptNode){

    xmlNode *cur_node = NULL;
    for (cur_node = a_node->children; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (strcmp ((char *)cur_node->name, "name") == 0){
                int size = (strlen ((char *) cur_node->children->content)) + 1; 
                wptNode->name = (char *) realloc (wptNode->name, size); 
                strcpy (wptNode->name, (char *) cur_node->children->content);
            }
            else{
                int len = strlen ((char*) cur_node->children->content) + 1; 
                GPXData *tmp = (GPXData *) malloc ((sizeof (GPXData)) + len);
                strcpy (tmp->value, (char*) cur_node->children->content); 
                strcpy (tmp->name, (char *) cur_node->name); 
                insertBack (wptNode->otherData, (void *)tmp); 
            }
        }
    }
}

/**
 * used by wpt/trkpt/rtept 
 * looks through current node for attributes
 * and sets the current node passed in's attributes 
 * to those found (MUST EXIST!)
 * called from trk->trkseg->wpthelper = trkpt level in for loop
 * or wpthelper = wpt level in for loop
 * or childCheckRte = rtept level in for loop 
 */
void attrCheck (xmlNode *a_node, Waypoint *wptNode){

    xmlAttr *attr;
    for (attr = a_node->properties; attr != NULL; attr = attr->next){
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name; 
        char *cont = (char *)(value->content); 
        if (strcmp (attrName, "lat") == 0){
            double val = strtod (cont, NULL); 
            wptNode->latitude = val; 
        }
        else if (strcmp (attrName, "lon") == 0){
            double val = strtod (cont, NULL); 
            wptNode->longitude = val; 
        }
    }
}

/**
 * starts at <gpx> level with node passed in 
 * passed in list is doc->routes
 * go down to rte level in for loop 
 * if rte is found, create a new route 
 * pass current node level (route) and 
 * route node into childCheckRte 
 * add updated route node into the 
 * route list 
 */
void rteHelper (xmlNode *a_node, List *builder){

    xmlNode *cur_node = NULL;  
    for (cur_node = a_node->children; cur_node != NULL; cur_node = cur_node->next){
        if (cur_node->type == XML_ELEMENT_NODE){
            if (strcmp ((char *)cur_node->name, "rte") == 0){
                Route *tmp = createRTE(); 
                childCheckRTE (cur_node, tmp);
                insertBack (builder, tmp);
                
            }
        }
    }
}

/**
 * receives node at rte level 
 * for loop goes to children level (name/rtept list/other)
 * if name, set name for route node passed in 
 * if otherdata set other data for route node passed in 
 * if rtept, create a new waypoint 
 * run attribute checker (which expects wpt level)
 * run rteptHelper which sets up the waypoint (name/otherdata)
 * add the waypoint to the rteNode->waypoint list 
 */

void childCheckRTE (xmlNode *a_node, Route *rteNode){
    xmlNode *cur_node = NULL;
    for (cur_node = a_node->children; cur_node != NULL; cur_node = cur_node->next){
        if (cur_node->type == XML_ELEMENT_NODE){
            if (strcmp ((char *)cur_node->name, "name") == 0){
                int size = (strlen ((char *) cur_node->children->content)) + 1; 
                rteNode->name = (char *) realloc (rteNode->name, size); 
                strcpy (rteNode->name, (char *) cur_node->children->content); 
            }
            else if (strcmp ((char *)cur_node->name, "rtept") == 0){
                Waypoint *tmpWPT = createWPT (); 
                attrCheck (cur_node, tmpWPT); 
                rteptHelper(cur_node, tmpWPT);
                insertBack (rteNode->waypoints, (void *)tmpWPT);
            }
            else{
                int otherLen = strlen ((char *) cur_node->children->content) + 1; 
                GPXData *tmp = (GPXData *) malloc ((sizeof (GPXData)) + otherLen); 
                strcpy (tmp->value, (char*)cur_node->children->content); 
                strcpy (tmp->name, (char *)cur_node->name); 
                insertBack(rteNode->otherData, (void * ) tmp); 
            }
        }
    }
}

/**
 * passes in node at rtept level (wpt level) 
 * for loop goes into children to look for a name or other data 
 * adds this to the waypoint passed in which will be added 
 * to the list of waypoints in route 
 */

void rteptHelper (xmlNode *a_node, Waypoint *tmpWPT){ //start at current node and look for wpt data in current 
    xmlNode *cur_node = NULL; 
    for (cur_node = a_node->children; cur_node != NULL; cur_node = cur_node->next){ //at rtept level 
        if (cur_node->type == XML_ELEMENT_NODE){
            if (strcmp ((char*)cur_node->name, "name") == 0){ //if waypoint has a name
                int size = (strlen ((char *)cur_node->children->content)) + 1; 
                tmpWPT->name = (char *) realloc (tmpWPT->name, size); 
                strcpy (tmpWPT->name, (char *) cur_node->children->content); 
            }
            else {
                int len = strlen ((char *) cur_node->children->content) + 1; 
                GPXData *tmp = (GPXData *) malloc ((sizeof (GPXData)) + len); 
                strcpy (tmp->value, (char *) cur_node->children->content); 
                strcpy (tmp->name, (char *)cur_node->name); 
                insertBack (tmpWPT->otherData, (void *)tmp); 
            }
        }
    }
}

void trkHelper (xmlNode *a_node, List *builder){

    xmlNode *cur_node = NULL; 
    for (cur_node = a_node->children; cur_node != NULL; cur_node = cur_node->next){
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (strcmp ((char *)cur_node->name, "trk") == 0){
                Track *tmp = createTRK(); 
                childCheckTRK (cur_node, tmp);
                insertBack (builder, tmp);
            }
        }        
    }
}

void childCheckTRK (xmlNode *a_node, Track *trkNode) {

    xmlNode *cur_node = NULL; 
    for (cur_node = a_node->children; cur_node != NULL; cur_node = cur_node->next){
        if (cur_node->type == XML_ELEMENT_NODE){
            if (strcmp ((char *)cur_node->name, "name") == 0){
                int size = (strlen ((char *) cur_node->children->content)) + 1; 
                trkNode->name = (char *) realloc (trkNode->name, size);
                strcpy (trkNode->name, (char *)cur_node->children->content);
            }
            else if (strcmp ((char *)cur_node->name, "trkseg") == 0){
                TrackSegment *tmpSeg = createSeg (); 
                wptHelper (cur_node, tmpSeg->waypoints, "trkpt"); //similar to doc->waypointlist level -> no name, just a list of waypoints 
                insertBack (trkNode->segments, tmpSeg);
            }
            else {
                int otherLen = strlen ((char *)cur_node->children->content) + 1; 
                GPXData *tmp = (GPXData *) malloc ((sizeof (GPXData)) + otherLen); 
                strcpy (tmp->value, (char *)cur_node->children->content); 
                strcpy (tmp->name, (char *)cur_node->name); 
                insertBack (trkNode->otherData, (void *)tmp); 
            }
        }
    }
}

/**Quick explanation of above
 * You can reuse the wpt function with track because 
 * at the tracksegment level, there are no names and just a
 * list of waypoints just like the gpx doc level where 
 * it is called for wpt 
 * For rte, the rte level has name and other data attributes
 */


/**
 * checks the GPXdoc passin for validity 
 * -> if a GPXdoc is valid, an xml tree can be built from the structure 
 * -> used in validateGPXdoc to check if GPXdoc passed in is valid and thus can have a tree built based on it 
 * ->-> to then be confirmed if it agrees with the GPX schema using the helper function validateTreeHelper
 */
bool docValidity(GPXdoc *doc){
    if (doc == NULL){
        return false; 
    }

    if (strcmp (doc->namespace, "") == 0){
        return false; 
    }
    else if (doc->version < 0){ //NOT 100% ABOUT THIS ONE -> UNINITIALIZED VALUE COULD VERY WELL BE OVER 0 
        return false; 
    }
    else if (doc->creator == NULL){
        return false; 
    }
    else if (strcmp (doc->creator, "") == 0){
        return false; 
    }
    else if (doc->waypoints == NULL){
        return false; 
    }
    else if (doc->routes == NULL){
        return false; 
    }
    else if (doc->tracks == NULL){
        return false; 
    }
    
    /*validating waypoints*/
    if (doc->waypoints->length > 0){ //check validity of every way point 
        //printf ("This length: %d\n", doc->waypoints->length); 
        ListIterator iter = createIterator (doc->waypoints); 
        void *elem; 
        while ((elem = nextElement (&iter)) != NULL){
            Waypoint *wptTmp = (Waypoint *)elem; 
            if (wptTmp->name == NULL){
                return false; 
            }
            if ((wptTmp->latitude < -90) || (wptTmp->latitude > 90) || (wptTmp->longitude < -180) || (wptTmp->longitude > 180)){
                return false; 
            }
            if (wptTmp->otherData == NULL){
                return false; 
            }
            if (wptTmp->otherData->length > 0){
                ListIterator iter2 = createIterator (wptTmp->otherData); 
                void *elem2; 
                while ((elem2 = nextElement (&iter2)) != NULL){
                    GPXData *tmpOther = (GPXData *)elem2; 
                    if ((strcmp (tmpOther->name, "") == 0) || (strcmp (tmpOther->value, "") == 0)){
                        return false; 
                    }
                }
            }
        }
    }
    /*done validating waypoints*/

    /*validating routes*/
    if (doc->routes->length > 0){
        ListIterator iter = createIterator (doc->routes); 
        void *elem; 
        while ((elem = nextElement(&iter)) != NULL){
            Route *tmpRte = (Route *)elem; 
            if (tmpRte->name == NULL){
                return false; 
            }
            if (tmpRte->otherData == NULL){
                return false; 
            }
            if (tmpRte->otherData->length > 0){
                ListIterator iter2 = createIterator (tmpRte->otherData); 
                void *elem2; 
                while ((elem2 = nextElement (&iter2))!= NULL){
                    GPXData *tmpOther = (GPXData *)elem2; 
                    if ((strcmp (tmpOther->name, "") == 0) || (strcmp (tmpOther->value, "") == 0)){
                        return false; 
                    }
                }
            }
            if (tmpRte->waypoints == NULL){ 
                return false; 
            }
            if (tmpRte->waypoints->length > 0){
                ListIterator iter2 = createIterator (tmpRte->waypoints); 
                void *elem2;
                while ((elem2 = nextElement (&iter2)) != NULL){
                    Waypoint *wptTmp = (Waypoint *)elem2; 
                    if (wptTmp->name == NULL){
                        return false; 
                    }
                    if ((wptTmp->latitude < -90) || (wptTmp->latitude > 90) || (wptTmp->longitude < -180) || (wptTmp->longitude > 180)){
                        return false; 
                    }
                    if (wptTmp->otherData == NULL){
                        return false; 
                    }
                    if (wptTmp->otherData->length > 0){
                        ListIterator iter3 = createIterator (wptTmp->otherData); 
                        void *elem3; 
                        while ((elem3 = nextElement (&iter3)) != NULL){
                            GPXData *tmpOther = (GPXData *)elem3; 
                            if ((strcmp (tmpOther->name, "") == 0) || (strcmp (tmpOther->value, "") == 0)){
                                return false; 
                            }
                        }
                    }

                }
            }
        }
    }
    /*done validating routes*/

    /*validating tracks*/ 
    if (doc->tracks->length > 0){
        ListIterator iter = createIterator (doc->tracks); 
        void *elem; 
        while ((elem = nextElement(&iter)) != NULL){
            Track *tmpTrk = (Track *)elem; 
            if (tmpTrk->name == NULL){
                return false; 
            }
            if (tmpTrk->otherData == NULL){
                return false; 
            }
            if (tmpTrk->otherData->length > 0){
                ListIterator iter2 = createIterator (tmpTrk->otherData); 
                void *elem2; 
                while ((elem2 = nextElement (&iter2)) != NULL){
                    GPXData *tmpOther = (GPXData *)elem2; 
                    if ((strcmp (tmpOther->name, "") == 0) || (strcmp (tmpOther->value, "") == 0)){
                        return false; 
                    }
                }
            }
            if (tmpTrk->segments == NULL){
                return false; 
            }
            if (tmpTrk->segments->length > 0){
                ListIterator iter2 = createIterator (tmpTrk->segments); 
                void *elem2; 
                while((elem2 = nextElement (&iter2)) !=NULL ){
                    TrackSegment *tmpSeg = (TrackSegment *)elem2; 
                    if (tmpSeg->waypoints == NULL){
                        return false; 
                    }
                    if (tmpSeg->waypoints->length > 0){
                        ListIterator iter3 = createIterator (tmpSeg->waypoints); 
                        void *elem3; 
                        while ((elem3 = nextElement (&iter3)) != NULL){
                            Waypoint *wptTmp = (Waypoint *)elem3; 
                            if (wptTmp->name == NULL){ 
                                return false; 
                            }
                            if ((wptTmp->latitude < -90) || (wptTmp->latitude > 90) || (wptTmp->longitude < -180) || (wptTmp->longitude > 180)){
                                return false; 
                            }
                            if (wptTmp->otherData == NULL){
                                return false; 
                            }
                            if (wptTmp->otherData->length > 0){
                                ListIterator iter4 = createIterator (wptTmp->otherData); 
                                void *elem4; 
                                while ((elem4 = nextElement (&iter4)) != NULL){
                                    GPXData *tmpOther = (GPXData *)elem4; 
                                    if ((strcmp (tmpOther->name, "") == 0) || (strcmp (tmpOther->value, "") == 0)){
                                        return false; 
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true; 
}

/**
 * This validates an xml tree against an gpx schema
 * -> used by 2 functions: createValidGPXdoc and validateGPXdoc 
 * ->-> createValidGPXdoc already has a confirmed valid xml tree through XMLReadFIle (if it returns not NULL)
 * ->-> validateGPXdoc needs to confirm a valid gpx (using function above), build a list on it if valid (createTree function), then pass list into this function
 */
bool validateTreeHelper (xmlDocPtr doc, char *gpxSchemaFile) {
    if ((doc == NULL) || (gpxSchemaFile == NULL) || (strcmp (gpxSchemaFile, "") == 0)){
        return false; 
    }
    bool found = false;

    char ext[5] = ""; 
    for (int i = 0; i < strlen (gpxSchemaFile) + 1; i++){
        if ((gpxSchemaFile[i] == '.') && ((i + 3) < (strlen(gpxSchemaFile) + 1)) && (i > 1)){
            ext[0] = gpxSchemaFile[i];
            ext[1] = gpxSchemaFile[i + 1]; 
            ext[2] = gpxSchemaFile[i + 2]; 
            ext[3] = gpxSchemaFile[i + 3]; 
            found = true;
            break; 
        }
    }
    if (found == true){
        ext[4] = '\0'; 
    }
    if (strcmp (ext, ".xsd") != 0){
        return false; 
    }  
    

    xmlSchemaPtr schema = NULL; 
    xmlSchemaParserCtxtPtr ctxt; 
    bool retVal = false; 

    xmlLineNumbersDefault (1); //enables line numbers -> do I need this? 

    ctxt = xmlSchemaNewParserCtxt (gpxSchemaFile); 

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt); //parse the scheme file and store in the scheme ptr
    xmlSchemaFreeParserCtxt(ctxt); //no longer need parser because XSD has been read in 

    if (doc == NULL){ 
        retVal = false; 
    }
    else {
        xmlSchemaValidCtxtPtr ctxt;
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, doc); 
        if (ret == 0){
            retVal = true; 
        }
        else if (ret > 0){
            retVal = false; 
        }
        else{
            retVal = false; 
        }
        xmlSchemaFreeValidCtxt(ctxt);
    }
    if (schema != NULL){
        xmlSchemaFree (schema); 
    }

    xmlSchemaCleanupTypes();
    return retVal;

}

/**
 * This function creates a xmlTree based on a GPXdoc passed in 
 * -> used by validate to build the tree once the GPXdoc has been confirmed to be valid so the tree can be passed to validateGPXdoc 
 * -> used by writeGPXDoc function to create a tree from a GPXdoc then write it out to a file
 */
xmlDocPtr createTree (GPXdoc *toWrite){
    if (toWrite == NULL){
        return NULL; 
    }
    /*creating xml doc to output into file with the data in gpxdoc*/ 
    xmlDocPtr doc = NULL; //used for the doc itself 
    xmlNodePtr root_node = NULL; //used to set root 
    xmlNodePtr node = NULL; //used to set children of root
    xmlNodePtr node1 = NULL; //currently used to set children at rtept level (route = node, rtept = node1); 
    xmlNodePtr node2 = NULL; //needed for trkpt depth 
    
    LIBXML_TEST_VERSION;

    doc = xmlNewDoc (BAD_CAST "1.0"); //creating the doc for the tree  
    root_node = xmlNewNode (NULL, BAD_CAST "gpx"); //creating a root node

    char ver[4];
    sprintf (ver, "%.1f", toWrite->version);
    xmlNewProp (root_node, BAD_CAST "version", BAD_CAST ver); 

    xmlNewProp (root_node, BAD_CAST "creator", BAD_CAST toWrite->creator); 

    xmlNsPtr nameSpace = xmlNewNs (root_node, (unsigned char *)toWrite->namespace, NULL);
    xmlSetNs (root_node, nameSpace); 

    xmlDocSetRootElement (doc, root_node); //set the root node for the doc
    
    char lon[12]; //ASK IN LAB
    char lat[12]; 

    //max long = -180.123456 = 11 + '\0' = 12
    /**
     * Dealing with waypoints (doc->waypoints) 
     * First loop goes through waypoint list in doc 
     * if a name is found, store the name as a child of the node 
     * Lon/lat must exist so start those as properties of the node 
     * loop through otherData of waypoint and set anything found as children of the waypoint
     */
    ListIterator iter = createIterator (toWrite->waypoints); 
    void *elem; 
    while ((elem = nextElement (&iter)) != NULL){
        Waypoint *wptTmp = (Waypoint *)elem; 
        node = xmlNewChild (root_node, NULL, BAD_CAST "wpt", NULL); //node is set to wpt (child of root_node) 
        if (strcmp(wptTmp->name, "") != 0){
            //add child for name
            xmlNewChild (node, NULL, BAD_CAST "name", (unsigned char *)wptTmp->name); 
        }
        sprintf (lat, "%.6f", wptTmp->latitude); 
        sprintf (lon, "%.6f", wptTmp->longitude); 
        xmlNewProp (node, BAD_CAST "lat", BAD_CAST (unsigned char *)lat); 
        xmlNewProp (node, BAD_CAST "lon", BAD_CAST (unsigned char *)lon); 
        ListIterator iter2 = createIterator (wptTmp->otherData); 
        void *elem2; 
        while ((elem2 = nextElement (&iter2)) != NULL){
            GPXData *otherTmp = (GPXData *)elem2; 
            //printf ("This: '%s'/'%s'\n", otherTmp->name, otherTmp->value); 
            xmlNewChild (node, NULL, BAD_CAST otherTmp->name , BAD_CAST otherTmp->value); 
        }
    }
    /* Done with Waypoints */ 

    /**
     * Dealing with routes (doc->routes)
     * check route for name, if it exists store it as a child of the rte 
     * check for route otherData, if it exists store it as children of rte 
     * look for waypoints (rtept) of route and if found 
     * set the name if it exists, the lon/lat, and loop through
     * it's otherdata and set as child of rtept (node1) if found 
     */
    iter = createIterator (toWrite->routes); 
    elem = NULL; 
    while ((elem = nextElement(&iter)) != NULL){
        Route *rteTmp = (Route *)elem; 
        node = xmlNewChild (root_node, NULL, BAD_CAST "rte", NULL); //node is set to rte node (child of root_node)
        if (strcmp(rteTmp->name, "") != 0){
            xmlNewChild (node, NULL, BAD_CAST "name", (unsigned char *)rteTmp->name);
        }
        ListIterator iter2 = createIterator (rteTmp->otherData); 
        void *elem2; 
        while ((elem2 = nextElement (&iter2)) != NULL){
            GPXData *otherTmp = (GPXData *)elem2; 
            xmlNewChild (node, NULL, BAD_CAST otherTmp->name, BAD_CAST otherTmp->value); 
        }
        iter2 = createIterator (rteTmp->waypoints); 
        elem2 = NULL; 
        while ((elem2 = nextElement (&iter2)) != NULL){
            Waypoint *wptTmp = (Waypoint *)elem2; 
            node1 = xmlNewChild (node, NULL, BAD_CAST "rtept", NULL); //node1 is set to rtept node (child of rte) 
            //printf ("%.6f | %.6f\n", wptTmp->latitude, wptTmp->longitude); 
            if (strcmp (wptTmp->name, "") != 0){
                xmlNewChild (node1, NULL, BAD_CAST "name", (unsigned char *)wptTmp->name); 
            }
            strcpy (lon, ""); 
            strcpy (lat, ""); 
            sprintf (lat, "%.6f", wptTmp->latitude);
            sprintf (lon, "%.6f", wptTmp->longitude); 
            xmlNewProp (node1, BAD_CAST "lat", BAD_CAST (unsigned char *)lat); 
            xmlNewProp (node1, BAD_CAST "lon", BAD_CAST (unsigned char *)lon);

            ListIterator iter3 = createIterator (wptTmp->otherData);
            void *elem3; 
            while ((elem3 = nextElement (&iter3)) != NULL){
                GPXData *otherDataTmp = (GPXData *)elem3; 
                xmlNewChild (node1, NULL, BAD_CAST otherDataTmp->name, BAD_CAST otherDataTmp->value); 
            }
        }
    }
    /*Done setting up route*/ 

    /**
     * setting up track 
     * look for track name
     * look for otherdata of track and set as child of track if found 
     * look for track segments of track and set as children of track 
     * set node1 to track segment 
     * look for waypoints of track segment (can't have anything but this) 
     * if a waypoint (trkpt) check for name, check for otherData,
     * and set lon/lat
     */ 
    iter = createIterator (toWrite->tracks); 
    elem = NULL; 
    while ((elem = nextElement(&iter)) != NULL){
        Track *trkTmp = (Track *)elem; 
        node = xmlNewChild (root_node, NULL, BAD_CAST "trk", NULL); //node is a trk (child of root_node)
        if (strcmp (trkTmp->name, "") != 0){
            xmlNewChild (node, NULL, BAD_CAST "name", (unsigned char *) trkTmp->name); 
        }
        ListIterator iter2 = createIterator (trkTmp->otherData); 
        void *elem2; 
        while ((elem2 = nextElement(&iter2)) != NULL){
            GPXData *tmpOther = (GPXData *)elem2; 
            xmlNewChild (node, NULL, BAD_CAST tmpOther->name, BAD_CAST tmpOther->value); 
        }
        elem2 = NULL; 
        iter2 = createIterator (trkTmp->segments); 
        while ((elem2 = nextElement (&iter2)) != NULL){
            TrackSegment *tmpSeg = (TrackSegment *)elem2; 
            node1 = xmlNewChild (node, NULL, BAD_CAST "trkseg", NULL); //node1 is trkseg (child of trk)
            ListIterator iter3 = createIterator (tmpSeg->waypoints); 
            void *elem3; 
            while ((elem3 = nextElement (&iter3))!= NULL){
                Waypoint *wptTmp = (Waypoint *)elem3; 
                node2 = xmlNewChild (node1, NULL, BAD_CAST "trkpt", NULL); //node2 is trkpt (child of trkseg)
                if (strcmp(wptTmp->name, "") != 0){
                    xmlNewChild (node2, NULL, BAD_CAST "name", (unsigned char *)wptTmp->name); 
                }
                ListIterator iter4 = createIterator (wptTmp->otherData); 
                void *elem4; 
                while ((elem4 = nextElement (&iter4)) != NULL){
                    GPXData *tmpOtherData = (GPXData *)elem4; 
                    xmlNewChild (node2, NULL, BAD_CAST tmpOtherData->name, BAD_CAST tmpOtherData->value);  
                }
                strcpy (lon, ""); 
                strcpy (lat, ""); 
                sprintf (lat, "%.6f", wptTmp->latitude);
                sprintf (lon, "%.6f", wptTmp->longitude);
                xmlNewProp (node2, BAD_CAST "lat", BAD_CAST (unsigned char *)lat); 
                xmlNewProp (node2, BAD_CAST "lon", BAD_CAST (unsigned char *)lon);
            }
        }
    }
    /*done setting up track*/ 

    return doc; 
}

/**
 * takes a xml tree and a file name and tries to write out the tree into the file
 */
bool writeOutHelper (xmlDocPtr doc, char *fileName) {
    if ((doc == NULL) || (fileName == NULL) || (strcmp (fileName, "") == 0)){
        return false; 
    }

    char ext[5] = ""; 
    for (int i = 0; i < strlen(fileName) + 1; i++){ //loop up to and including null terminator
        if (fileName[i] == '.' && (i > 1)){
            ext[0] = fileName[i]; 
            ext[1] = fileName[i + 1]; 
            ext[2] = fileName[i + 2];
            ext[3] = fileName[i + 3]; 
            break; 
        }
    }
    ext[4] = '\0'; 
    if (strcmp (ext, ".gpx") != 0){
        return false; 
    }
    xmlSaveFormatFileEnc (fileName, doc, "UTF-8", 1); 
    return true; 
}

float haversine (float firstLat, float firstLon, float secondLat, float secondLon){
    float lat1 = firstLat; 
    float lon1 = firstLon; 

    float lat2 = secondLat;  
    float lon2 = secondLon; 
    
    float radLat1 = lat1 * (M_PI / 180); 
    float radLat2 = lat2 * (M_PI / 180);

    float deltaLat = (lat2 - lat1) * (M_PI / 180); 
    float deltaLon = (lon2 - lon1) * (M_PI / 180);

    float a = sin(deltaLat / 2) * sin(deltaLat / 2) + cos(radLat1) * cos (radLat2) * sin (deltaLon/2) * sin (deltaLon/2);  

    float c = 2 * atan2l (sqrt(a), sqrt(1 - a));

    float d = 6371e3 * c; 
    return d; 
}

void dummyDelete (void *data){
    return; 
}