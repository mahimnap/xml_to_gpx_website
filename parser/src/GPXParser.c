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
 * takes a filename and checks if it is NULL or name is empt 
 * if so return NULL otherwise continue
 * tries to run tree parser on GPX format 
 * if it's NULL, it was unsuccessful so return 
 *      - for a1 we can assume successful xml = gpx
 * Malloc space for the GPXDoc 
 * strcpy the namespace to doc by using ns/href to retrieve 
 * version and creator are treated as attributes
 * malloc space for them based on length and set appropriate
 * doc members 
 * initialize 3 lists for GPXdoc with list api 
 * call WPT/TRK/RTE helper functions to build each list 
 * based on the GPX file 
 * Free the tree and created by doc
 * return the completed gpxdoc if everything was successful 
 */
GPXdoc *createGPXdoc (char *fileName){

    xmlDoc *doc = NULL; 
    xmlNode *root_element = NULL; 
    xmlNode *current = NULL; 
    if ((fileName == NULL) || (strcmp (fileName, "") == 0)){
        return NULL; 
    }
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL){ 
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL; 
    }

    root_element = xmlDocGetRootElement (doc); 

    GPXdoc *newParse = (GPXdoc *) malloc (sizeof (GPXdoc));
    strcpy (newParse->namespace, (char*)root_element->ns->href); 
    current = root_element; 
    xmlAttr *attr; 

    for (attr = current->properties; attr != NULL; attr = attr->next){
        xmlNode *value = attr->children; 
        char *attrName = (char *)attr->name; 
        if (strcmp (attrName, "version") == 0){
            char *cont = (char *)(value->content);
            newParse->version = strtod (cont, NULL);
        } 
        else if (strcmp (attrName, "creator") == 0){
            char *cont = (char *)(value->content);
            newParse->creator = (char *) malloc (strlen (cont) + 1);
            strcpy (newParse->creator, cont);
        }
    }

    newParse->waypoints = initializeList (&waypointToString, &deleteWaypoint, &compareWaypoints);
    newParse->routes = initializeList (&routeToString, &deleteRoute, &compareRoutes); 
    newParse->tracks = initializeList (&trackToString, &deleteTrack, &compareTracks); 

    wptHelper (current, newParse->waypoints, "wpt");
    rteHelper (current, newParse->routes); 
    trkHelper (current, newParse->tracks); 

    xmlFreeDoc(doc); 
    xmlCleanupParser(); 

    return newParse;
}

/**
 * utilized sprintf as well as careful calculations 
 * of bytes necessary for my desired toString format
 * through the use of strlen and definition 
 * of constant sizes for strings to be append/added
 * (things like tab, newline and other general formatting)
 */
char* GPXdocToString(GPXdoc* doc){

    if (doc == NULL){
        return NULL;
    }

    int sizeNS = strlen (doc->namespace); 
    int sizeVer = 3; //1.1 = 3 chars -> DOUBLE CHECK THIS IN LAB 
    int sizeCtr = strlen (doc->creator);
    int otherChars = 3; //'\n', '\n', '\n' 
    int totalSize = sizeNS + sizeVer + sizeCtr + otherChars; 
   
    char *docStr = (char *) malloc (totalSize + 1);
    sprintf (docStr, "%s\n%.1f\n%s\n", doc->namespace, doc->version, doc->creator); 

    void *element; 
    ListIterator iter = createIterator (doc->waypoints); 
    int wptListSize = doc->waypoints->length;
    char *allWPT[wptListSize]; 
    int finalSize = 0; 
    int i = 0; 

    while ((element = nextElement(&iter)) != NULL){
        Waypoint *itrTmp = (Waypoint *) element; 
        char *holder = doc->waypoints->printData(itrTmp); 
        allWPT[i] = holder; 
        finalSize += strlen(allWPT[i]); 
        i++; 
    }

    finalSize += totalSize; 
    iter = createIterator (doc->routes); 
    int rteListSize = doc->routes->length;
    char *allRTE[rteListSize]; 
    i = 0; 

    while ((element = nextElement(&iter))!= NULL){
        Route *itrTmp = (Route *) element; 
        char *holder = doc->routes->printData(itrTmp); 
        allRTE[i] = holder; 
        finalSize += strlen (allRTE[i]); 
        i++; 
    }

    iter = createIterator (doc->tracks); 
    int trkListSize = doc->tracks->length; 
    char *allTRK[trkListSize]; 
    i = 0; 

    while ((element = nextElement(&iter)) != NULL){
        Track *itrTmp = (Track *) element; 
        char *holder = doc->tracks->printData(itrTmp);
        allTRK[i] = holder; 
        finalSize += strlen (allTRK[i]); 
        i++;
    }

    char *retStr = (char *) malloc (finalSize + 1); 
    strcpy (retStr, docStr);

    for (int i = 0; i < wptListSize; i++){ 
        strcat (retStr, allWPT[i]);
        free (allWPT[i]); 
    }
    
    for (int i = 0; i < rteListSize; i++){
        strcat (retStr, allRTE[i]); 
        free (allRTE[i]); 
    }

    for (int i = 0; i < trkListSize; i++){
        strcat (retStr, allTRK[i]); 
        free (allTRK[i]); 
    }

    free (docStr); 
    return retStr; 
}

/**
 * free the entire doc by first freeing it's 
 * attributes, then all of its lists with 
 * the freeList function from the list api 
 */
void deleteGPXdoc(GPXdoc* doc) {

    if (doc == NULL){
        return; 
    }

    free (doc->creator); 
    freeList (doc->waypoints); 
    freeList (doc->routes); 
    freeList (doc->tracks); 
    free (doc); 
}

/**
 * all delete functions are self-explanitory 
 * will be used by tree but can also be used 
 * by user if they're looking to 
 * extract specific nodes from the list
 */
void deleteTrack(void* data) {

    Track *tmp = data; 

    if (tmp == NULL){
        return; 
    }

    free (tmp->name); 
    freeList (tmp->segments); 
    freeList(tmp->otherData); 
    free (tmp); 
}

/**
 * toString has same logic as above 
 */
char* trackToString(void* data){

    Track *tmp = (Track *) data; 

    if (tmp == NULL){
        return NULL; 
    }

    char namePrefix[] = "\ttrk name: "; 
    int nameSize = strlen (tmp->name) + strlen (namePrefix) + 1; //1 for appending a '\n' 
    char *nameStr = (char *) malloc (nameSize + 1); //1 for null 
    sprintf (nameStr, "%s%s\n", namePrefix, tmp->name);

    int numSeg = tmp->segments->length; 
    char *listSegPtr[numSeg]; 
    int sizeSeg = 0; 
    void *element; 
    int i = 0; 
    ListIterator iter = createIterator (tmp->segments); 

    while ((element = nextElement (&iter)) != NULL){
        TrackSegment *itrTmp = (TrackSegment *)element; 
        char *tmpSeg = tmp->segments->printData(itrTmp); 
        listSegPtr[i] = tmpSeg; 
        sizeSeg += strlen (listSegPtr[i]); 
        i++; 
    }

    int numOther = tmp->otherData->length; 
    char *listOtherPtr[numOther]; 
    int sizeOther = 0; 
    i = 0; 
    iter = createIterator (tmp->otherData); 

    while ((element = nextElement (&iter)) != NULL){
        GPXData *itrTmp = (GPXData *) element; 
        char *holder = tmp->otherData->printData (itrTmp);
        listOtherPtr[i] = holder; 
        sizeOther += strlen (listOtherPtr[i]); 
        i++; 
    }

    int finalSize = nameSize + sizeSeg + sizeOther;
    char *retStr = (char *) malloc (finalSize + 1); //+1 for null 
    strcpy (retStr, nameStr); 

    for (int i = 0; i < numOther; i++){
        strcat (retStr, listOtherPtr[i]); 
        free (listOtherPtr[i]); 
    }

    for (int i = 0; i < numSeg; i++){
        strcat (retStr, listSegPtr[i]); 
        free (listSegPtr[i]); 
    }

    free (nameStr); 
    return retStr; 
}

/**
 * comparing addresses for all of my compare functions 
 * to see if two pointers are referecing the same 
 * address (i.e. node)
 */
int compareTracks(const void *first, const void *second){

    if (first == NULL || second == NULL){
        return 0; 
    }

    if (&first == &second){
        return 1; 
    }
    else {
        return 0; 
    }
}

void deleteTrackSegment(void* data){

    TrackSegment *tmp = data; 
    if (tmp == NULL){
        return; 
    }
    freeList (tmp->waypoints); 
    free (tmp); 
}

char* trackSegmentToString(void* data){  //-> 2 tabs in 

    TrackSegment *tmp = (TrackSegment *)data; 
    if (tmp == NULL){
        return NULL; 
    }
    
    char segName[] = "\t\ttrkseg\n"; //no name children for trackseg 
    int numWPT = tmp->waypoints->length; 
    char *listWptPtr[numWPT]; 
    int sizeWPT = 0;

    void *element; 
    ListIterator iter = createIterator (tmp->waypoints); 
    int i = 0; 
    while ((element = nextElement(&iter)) != NULL){
        Waypoint *itrTmp = (Waypoint *) element; 
        char *tmpWPT = tmp->waypoints->printData (itrTmp); 
        char *addTab = (char *)malloc (strlen (tmpWPT) + 3); //3-> 2 for tabs 1 for null 
        sprintf (addTab, "\t\t%s", tmpWPT); 
        free (tmpWPT); 
        listWptPtr [i] = addTab; 
        sizeWPT += strlen (listWptPtr[i]); 
        i++; 
    }

    int finalSize = sizeWPT + (strlen (segName)) + 1; 
    char *retStr = (char *) malloc (finalSize); 
    strcpy (retStr, segName); 

    for (int i = 0; i < numWPT; i++){
        strcat (retStr, listWptPtr[i]); 
        free (listWptPtr[i]); 
    }

    return retStr; 
}

int compareTrackSegments(const void *first, const void *second){

    if (first == NULL || second == NULL){
        return 0; 
    }

    if (&first == &second){
        return 1; 
    }
    else {
        return 0; 
    }
}

void deleteGpxData (void *data){
    
    GPXData* tmpData; 

    if (data == NULL){
        return; 
    }

    tmpData = (GPXData*) data; 
    free (tmpData); //will free struct and array[] -> flexible array lecture 
}

int compareGpxData(const void *first, const void *second){

    if (first == NULL || second == NULL){
        return 0; 
    }

    if (&first == &second){
        return 1; 
    }
    else {
        return 0; 
    }
}

char* gpxDataToString( void* data){ 

    GPXData *tmp = (GPXData *) data;

    if (tmp == NULL){
        return NULL; 
    }

    int lenNm = strlen (tmp->name) + 1; 
    int lenVal = strlen (tmp->value) + 1; 
    int delimChars = 8; //'\t', '\t', '\t', '\t' ' ', '=', ' ', '\n' 
    int totalSize = lenNm + lenVal + delimChars; 
    //if (strcmp tmp->parent->name)
    char *retStr = (char *) malloc (totalSize); 
    sprintf (retStr, "\t\t\t\t%s = %s\n", tmp->name, tmp->value);

    return retStr; 
}

void deleteRoute(void* data){

    Route *tmp = (Route *)data; 

    if (tmp == NULL) {
        return; 
    }

    free (tmp->name); 
    freeList (tmp->waypoints); 
    freeList (tmp->otherData); 
    free (tmp); 
}

char* routeToString(void* data){

    Route *tmp = (Route *) data; 
    
    if (tmp == NULL){
        return NULL;
    }

    int nameLen = strlen (tmp->name); 
    int otherChars = 12; 
    int myNameStrSize = nameLen + otherChars; 
    char *nameStr = (char *) malloc (myNameStrSize  + 1);
    //'\t'(1) + 'rte name: x'(10)  + '\n' (1) 

    sprintf (nameStr, "\trte name: %s\n", tmp->name);

    int numWPT = tmp->waypoints->length; 
    char *listWptPtr[numWPT]; 
    int sizeWPT = 0; 

    void *element; 
    ListIterator iter = createIterator (tmp->waypoints); 
    int i = 0; 

    while ((element = nextElement(&iter)) != NULL){
        Waypoint *itrTmp = (Waypoint *) element; 
        char *tmpWPT = tmp->waypoints->printData (itrTmp); //SAME AS -> char *tmpWPT = waypointToString (itrTmp);
        char *addTab = (char *) malloc (strlen (tmpWPT) + 2); //+2 -> 1 for null and 1 for extra tab 
        sprintf (addTab, "\t%s", tmpWPT); 
        free (tmpWPT); 
        listWptPtr[i] = addTab; 
        sizeWPT += strlen (listWptPtr[i]); 
        i++; 
    }

    int numOther = tmp->otherData->length; 
    char *listOtherPtr[numOther];
    int sizeOther = 0; 

    iter = createIterator (tmp->otherData); 
    i = 0; 

    while ((element = nextElement(&iter)) != NULL){ //otherData List
        GPXData *itrTmp = (GPXData *) element; 
        char *holder = tmp->otherData->printData (itrTmp); 
        listOtherPtr[i] = holder; 
        sizeOther += strlen (listOtherPtr[i]); 
        i++; 
    }
    
    int finalSize = myNameStrSize + sizeWPT + sizeOther; 
    char *retStr = (char *) malloc (finalSize + 1); 
    strcpy (retStr, nameStr); 

    for (int i = 0; i < numOther; i++){
        strcat (retStr, listOtherPtr[i]); 
        free (listOtherPtr[i]); 
    }

    for (int i = 0; i < numWPT; i++){
        strcat (retStr, listWptPtr[i]); 
        free (listWptPtr[i]); 
    }

    free (nameStr); 

    return retStr; 
}

int compareRoutes(const void *first, const void *second){

    if (first == NULL || second == NULL){
        return 0; 
    }

    if (&first == &second){
        return 1; 
    }
    else {
        return 0; 
    }
}

void deleteWaypoint(void* data) {

    Waypoint *tmp = (Waypoint *)data; 

    if (tmp == NULL){
        return; 
    }

    free (tmp->name);
    freeList (tmp->otherData);
    free(tmp); 
}

char* waypointToString(void* data) {

    Waypoint *tmp = (Waypoint *) data; 

    if (tmp == NULL){
        return NULL;
    }

    char lonStr [MAX_LONLAT_LENGTH];
    char latStr [MAX_LONLAT_LENGTH]; 
    //-180.0 <= value < 180.0 -> 5 is max (inc dec) + 6 after dec = 11 + 1 for null = 12
    sprintf (lonStr, "%lf", tmp->longitude); 
    sprintf (latStr, "%lf", tmp->latitude); 

    void *element; 
    ListIterator iter = createIterator (tmp->otherData);

    int wptNameLen = strlen (tmp->name); 
    int wptLonLen = strlen (lonStr);
    int wptLatLen = strlen (latStr);
    int otherChars = 30; 
    //x = skip: ('\t' = 1) + ('wpt name: x ' = 11) + ('[lon = x | lat = x]' = 17) + ('\n' = 1) = 30
    int totalSize = wptNameLen + wptLonLen + wptLatLen + otherChars + 1; 
    char *wptStr = (char *) malloc (totalSize); 
    sprintf (wptStr, "\twpt name: %s [lat = %s | lon = %s]\n", tmp->name, latStr, lonStr);
    int numDataList = tmp->otherData->length; 
    char *listElePtr[numDataList];
    int finalSize = 0; 
    int i = 0; 

    while ((element = nextElement(&iter)) != NULL){
        GPXData *itrTmp = (GPXData *) element;
        char *holder = tmp->otherData->printData(itrTmp);
        listElePtr[i] = holder; 
        finalSize += strlen (listElePtr[i]); 
        i++; 
    }

    finalSize += totalSize;//total size already accounts for NULL terminator
    char *retStr = (char *) malloc (finalSize); 
    strcpy (retStr, wptStr); 

    for (int i = 0; i < numDataList; i++){ 
        strcat (retStr, listElePtr[i]);
        free(listElePtr[i]); 
        //needs to be freed because toString functions return dynamically allocated strings
    }
    
    free(wptStr); 
    //can swtich wptStr to statically allocated to increase efficiency of code 

    return retStr;
}

int compareWaypoints(const void *first, const void *second){

    if (first == NULL || second == NULL){
        return 0; 
    }

    if (&first == &second){
        return 1; 
    }
    else {
        return 0; 
    }
}

/**
 * use list api function to return 
 * size of the lists for next 
 * 3 functions
 */
int getNumWaypoints(const GPXdoc* doc) {

    if (doc == NULL){
        return 0;
    }

    return doc->waypoints->length; 
}

int getNumRoutes(const GPXdoc* doc) {

    if (doc == NULL){
        return 0; 
    }

    return doc->routes->length; 
}

int getNumTracks(const GPXdoc* doc){

    if (doc == NULL){
        return 0; 
    }

    return doc->tracks->length; 
}

/**
 * loop through all Tracks in the 
 * list stored in doc 
 * and use the list api length 
 * attribute to add up all the 
 * segments at every Tracls
 */
int getNumSegments(const GPXdoc* doc) {

    if (doc == NULL){
        return 0; 
    }

    ListIterator iter = createIterator (doc->tracks); 
    void *elem; 
    int segCnt = 0; 

    while ((elem = nextElement(&iter)) != NULL){
        Track *tmpTrk = (Track *) elem; 
        segCnt += tmpTrk->segments->length; 
    }

    return segCnt; 
}

/**
 * iterate to wpt-type depth 
 * for all 3 (rte, wtp, trk)
 * to get the number of 
 * 'name's and otherList sizes
 */

int getNumGPXData(const GPXdoc* doc){

    if (doc == NULL){
        return 0; 
    }
    ListIterator iter = createIterator(doc->tracks); 
    void *elem; 
    int size = 0; 

    while ((elem = nextElement (&iter)) != NULL){
        Track *trkTmp = (Track *)elem;
        size += trkTmp->otherData->length; 
        if (strcmp(trkTmp->name, "") != 0) {
            size++; 
        }
        ListIterator iter2 = createIterator (trkTmp->segments);
        void *elem2;
        while ((elem2 = nextElement (&iter2)) != NULL){
            TrackSegment *segTmp = (TrackSegment *)elem2; 
            ListIterator iter3 = createIterator (segTmp->waypoints); 
            void *elem3; 
            while ((elem3 = nextElement (&iter3)) != NULL){
                Waypoint *wptTmp = (Waypoint *)elem3; 
                size += wptTmp->otherData->length; 
                if (strcmp(wptTmp->name, "") != 0){
                    size++; 
                }
            }
        }
    }

    iter = createIterator (doc->routes);

    while ((elem = nextElement (&iter)) != NULL){
        Route *rteTmp = (Route*) elem; 
        size += rteTmp->otherData->length; 
        //printf ("ROOT ELEMENT X HAS %d other data\n", rteTmp->otherData->length);
        if (strcmp (rteTmp->name, "") != 0){
            size++; 
        }
        ListIterator iter2 = createIterator (rteTmp->waypoints); 
        void *elem2; 
        while ((elem2 = nextElement (&iter2)) != NULL){
            Waypoint *wptTmp = (Waypoint *)elem2; 
            size += wptTmp->otherData->length; 
            //printf ("WAYPOINT ELEMENT X HAS %d other data\n", wptTmp->otherData->length);
            if (strcmp (wptTmp->name, "") != 0){
                size++; 
            }
        }
    } 

    iter = createIterator (doc->waypoints);

    while ((elem = nextElement(&iter)) != NULL){
        Waypoint *wptTmp = (Waypoint *)elem; 
        size += wptTmp->otherData->length; 
        if (strcmp (wptTmp->name, "") != 0){
            size++; 
        }
    }
    return size; 
}

/**
 * iterate through list until (if at all)
 * a match is found, then return
 * - same logic the rest
 */
Waypoint* getWaypoint(const GPXdoc* doc, char* name){

    if (doc == NULL || name == NULL){
        return NULL; 
    }

    ListIterator iter = createIterator (doc->waypoints); 
    void *elem; 

    while ((elem = nextElement (&iter)) != NULL){
        Waypoint *wptTmp = (Waypoint *)elem; 
        if ((strcmp (name, wptTmp->name)) == 0){
            return wptTmp; 
        }
    }

    return NULL; //default to NULL if nothing is found 
}
Track* getTrack(const GPXdoc* doc, char* name){

    if (doc == NULL || name == NULL){
        return NULL; 
    }

    ListIterator iter = createIterator (doc->tracks); 
    void *elem; 

    while ((elem = nextElement (&iter)) != NULL){
        Track *trkTmp = (Track *)elem; 
        if ((strcmp (name, trkTmp->name)) == 0){
            return trkTmp; 
        }
    }

    return NULL;
}


Route* getRoute(const GPXdoc* doc, char* name){

    if (doc == NULL || name == NULL){
        return NULL; 
    }

    ListIterator iter = createIterator (doc->routes);
    void *elem; 

    while ((elem = nextElement (&iter)) != NULL){
        Route *rteTmp = (Route*)elem; 
        if ((strcmp (name, rteTmp->name)) == 0){
            return rteTmp; 
        }
    }
    return NULL; 
}

/**
 * updated GPXdoc with helper function that validates against 
 * a schema file
 */
GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile){
    xmlDoc *doc = NULL; 
    xmlNode *root_element = NULL; 
    xmlNode *current = NULL; 
    if ((fileName == NULL) || (strcmp (fileName, "") == 0) || (strcmp (gpxSchemaFile, "") == 0) || (gpxSchemaFile == NULL)){
        return NULL; 
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
        return NULL; 
    }

    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL){ 
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump(); 
        return NULL; 
    }

    //have a valid xml file (xmlReadFile not NULL) so we need to validate against schema 
    if (validateTreeHelper(doc, gpxSchemaFile) == false){ 
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump(); 
        return NULL; 
    }

    root_element = xmlDocGetRootElement (doc); 

    GPXdoc *newParse = (GPXdoc *) malloc (sizeof (GPXdoc));
    strcpy (newParse->namespace, (char*)root_element->ns->href); 
    current = root_element; 
    xmlAttr *attr; 

    for (attr = current->properties; attr != NULL; attr = attr->next){
        xmlNode *value = attr->children; 
        char *attrName = (char *)attr->name; 
        if (strcmp (attrName, "version") == 0){
            char *cont = (char *)(value->content);
            newParse->version = strtod (cont, NULL);
        } 
        else if (strcmp (attrName, "creator") == 0){
            char *cont = (char *)(value->content);
            newParse->creator = (char *) malloc (strlen (cont) + 1);
            strcpy (newParse->creator, cont);
        }
    }

    newParse->waypoints = initializeList (&waypointToString, &deleteWaypoint, &compareWaypoints);
    newParse->routes = initializeList (&routeToString, &deleteRoute, &compareRoutes); 
    newParse->tracks = initializeList (&trackToString, &deleteTrack, &compareTracks); 

    wptHelper (current, newParse->waypoints, "wpt");
    rteHelper (current, newParse->routes); 
    trkHelper (current, newParse->tracks); 

    xmlFreeDoc(doc); 
    xmlCleanupParser(); 
    xmlMemoryDump(); 

    return newParse;
}

bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile) {

    if (doc == NULL || gpxSchemaFile == NULL || (strcmp (gpxSchemaFile, "") == 0)){
        return false; 
    }
    //write out tree to file and check if valid xml with xmlReadFile 

    if (docValidity(doc) == false){ //if it's a valid doc, you can use the information to build a valid xml tree
        return false; 
    }

    xmlDocPtr tree = createTree (doc); //valid doc so build a tree based on that doc 
    if (tree == NULL){
        xmlFreeDoc (tree); 
        xmlCleanupParser (); 
        xmlMemoryDump(); 
        return false; 
    }

    if (validateTreeHelper (tree, gpxSchemaFile) == false){ //validate that tree against the schema file 
        xmlFreeDoc (tree); 
        xmlCleanupParser (); 
        xmlMemoryDump(); 
        return false; 
    }
    //check2 = validateTreeHelper (doc, gpxSchemaFile); //validates xml tree against xsd file 

    xmlFreeDoc (tree); 
    xmlCleanupParser (); 
    xmlMemoryDump(); 

    return true; 
}

/**
 * doc passed in is a valid doc, and is a valid gpx file (both tested in validateGPXdoc)
 */
bool writeGPXdoc(GPXdoc* doc, char* fileName) {
    if ((doc == NULL) || (fileName == NULL) || (strcmp(fileName, "") == 0)){
        return false; 
    }
    
    xmlDocPtr tree = createTree (doc); 
    if (tree == NULL){
        xmlFreeDoc (tree); 
        xmlCleanupParser (); 
        xmlMemoryDump(); 
        return false; 
    }
    if (writeOutHelper (tree, fileName) == true){
        xmlFreeDoc (tree); 
        xmlCleanupParser (); 
        xmlMemoryDump(); 
        return true; 
    }
    else {
        xmlFreeDoc (tree); 
        xmlCleanupParser (); 
        xmlMemoryDump(); 
        return false; 
    }
}

float round10 (float len){
    int rnd = (len + 5) / 10; //404.9 + 5 = 409.5 / 10 = 40.95 -> int = 40 | 405.1 + 5 = 410.1 / 10 = 41.01 -> int = 41
    rnd = rnd * 10;  //40 * 10 = 400 | 41 * 10 = 410 
    return (float)rnd; 
}

/**
 * First Iteration -> sees that firstLat is "unitilized" (not really) then sets 
 * the first lon and lat 
 * Second Iteration -> initial values for first lon/lat have been set so now every iteration will begin 
 * at second Lon/Lat then move up to first then off the queue compare with the value stored at first values (with haversine -> distance) 
 * Last iteration: final value will shift to first val after comparison but then the loop will end -> desirable outcome ->last should not be 
 * compared with anything (first has something ahead, n2, but nothing before it, last has something before it but nothing after it)
 * Switch to for loop instead???
*/
float getRouteLen(const Route *rt){
    if (rt == NULL){
        return 0; 
    }
    float firstLat = 200; 
    float firstLon = 200; 
    float secondLat = 200; 
    float secondLon = 200; 
    float totalCount = 0; 

    ListIterator iter = createIterator (rt->waypoints);
    void *elem; 
    while ((elem = nextElement (&iter)) != NULL){
        Waypoint *tmpWpt = (Waypoint *)elem; 
        if (firstLat == 200){
            firstLat = tmpWpt->latitude; 
            firstLon = tmpWpt->longitude;
        }
        else {
            secondLat = tmpWpt->latitude; 
            secondLon = tmpWpt->longitude; 
            totalCount += haversine (firstLat, firstLon, secondLat, secondLon); 
            firstLat = secondLat; 
            firstLon = secondLon ; 
        }
    }
    return totalCount;  
}

/**
 * very similar logic as above just goes one level deeper in 
 * the tree to access waypoints (obviously)
 * and it sets distances between trkseg end and start points 
 * if they both exist in the current iteration 
 * -> doesn't actually change logic, just works with same 
 * implemenation because outer loop is looping through
 * tracksegments so the lat/lon values are held from the 
 * previous iteration -> pretty much treats it like a single 
 * list with the logic of the lat/lon vars 
 */
float getTrackLen(const Track *tr){
    if (tr == NULL){
        return 0; 
    }
    float firstLat = 200; 
    float firstLon = 200; 
    float secondLat = 200; 
    float secondLon = 200;

    float totalCnt = 0; 

    ListIterator iter = createIterator (tr->segments); 
    void *elem; 
    while ((elem = nextElement (&iter)) != NULL){
        TrackSegment *tmpSeg = (TrackSegment *)elem; 
        ListIterator iter2 = createIterator (tmpSeg->waypoints); 
        void *elem2; 
        while ((elem2 = nextElement(&iter2)) != NULL){
            Waypoint *tmpWpt = (Waypoint *)elem2; 
            if (firstLat == 200){
                firstLat = tmpWpt->latitude; 
                firstLon = tmpWpt->longitude; 
            }
            else {
                secondLat = tmpWpt->latitude; 
                secondLon = tmpWpt->longitude; 
                totalCnt += haversine (firstLat, firstLon, secondLat, secondLon);
                firstLat = tmpWpt->latitude; 
                firstLon = tmpWpt->longitude; 
            }
        }
    }
    return totalCnt; 
}

/**
 * looking for a route distance that is desirable (len) with a given tolerance (delta)
 * subract the distance of the route (getroutelen) by len and get the absolute value 
 * check if this value is within (<=) the tolerable range of delta
 */

int numRoutesWithLength(const GPXdoc* doc, float len, float delta){
    if (doc == NULL || len < 0 || delta < 0){
        return 0; 
    }

    int cnt = 0; 
    ListIterator iter = createIterator (doc->routes); 
    void *elem; 

    while ((elem = nextElement(&iter)) != NULL){
        Route *tmpRte = (Route *)elem; 

        float dif = fabsf (getRouteLen(tmpRte) - len); 

        if (dif <= delta){
            cnt ++; 
        }
    }
    return cnt; 
}

/**
 * same logic as above
 */
int numTracksWithLength(const GPXdoc* doc, float len, float delta){
    if ((doc == NULL) || (len < 0) || (delta < 0)){
        return 0; 
    }

    int cnt = 0; 
    ListIterator iter = createIterator (doc->tracks); 
    void *elem; 

    while ((elem = nextElement (&iter)) != NULL){
        Track *tmpTrk = (Track *)elem; 

        float dif = fabsf (getTrackLen(tmpTrk) - len); 

        if (dif <= delta){
            cnt++;
        } 
    }
    return cnt; 
}

/**
 * checks if route loops back to beginning 
 * beginning can be offset by delta -> this time 
 * we are using the harsine function to get distance 
 * and thus the magnitude is returned, so only 
 * have to check if the distance between start and end's 
 * magnitude (i.e. distance > delta -> return false
 * unlike in earlier function where we setup a range 
 * with desired len to search for
 * Accomplished by setting the start and end lon/lat 
 * to values outside possible range, going to the first 
 * waypoint element and storing it into start values (i = 0)
 * going to last element (i = route->waypoints->length - 1) and 
 * storing end value based on that waypoint 
 * finally get the distance between them with haversine 
 * and check if it's within delta's tolerance 
 */
bool isLoopRoute(const Route* route, float delta){
    if ((route == NULL) || (delta < 0) || (route->waypoints->length < 4)){
        return false; 
    }
    float startLat = 200;
    float startLon = 200; 
    float endLat = 200; 
    float endLon = 200; 

    ListIterator iter = createIterator (route->waypoints); 
    void *elem; 
    for (int i = 0; i < route->waypoints->length; i++){
        elem = nextElement (&iter); 
        Waypoint *tmpWpt = (Waypoint *)elem; 
        if (i == 0){
            startLat = tmpWpt->latitude; 
            startLon = tmpWpt->longitude; 
        }
        else if (i == ((route->waypoints->length) - 1)){
            endLat = tmpWpt->latitude; 
            endLon = tmpWpt->longitude; 
        } 
    }

    float distance = haversine (startLat, startLon, endLat, endLon); 
    if (distance <= delta) {
        return true; 
    }
    else {
        return false; 
    }
}

/**
 * similar logic as above, looking for first node in
 * first segment (i == 0, j == 0) to compare with final node 
 * in final segment (i == trk->segments->length -1, j == tmpSeg->waypoints->length - 1)
 * after setting them up, send to haversine to get distance, then see if it's in 
 * tolerable range of delta
 */
bool isLoopTrack(const Track *tr, float delta){
    if ((tr == NULL) || (delta < 0)){
        return false; 
    }
    float startLat = 200; 
    float startLon = 200; 
    float endLat = 200; 
    float endLon = 200; 
    int wptCnt = 0; 
    int i = 0; 
    int j = 0; 

    ListIterator iter = createIterator (tr->segments); 
    void *elem; 
    for (i = 0; i < tr->segments->length; i++){
        elem = nextElement (&iter); 
        TrackSegment *tmpSeg = (TrackSegment *)elem; 
        wptCnt += tmpSeg->waypoints->length; 

        ListIterator iter2 = createIterator (tmpSeg->waypoints); 
        void *elem2; 

        for (j = 0; j < tmpSeg->waypoints->length; j++){
            elem2 = nextElement (&iter2); 
            Waypoint *tmpWpt = (Waypoint *)elem2; 

            if ((j == 0) && (i == 0)){
                startLat = tmpWpt->latitude; 
                startLon = tmpWpt->longitude; 
            }
            else if ((i == (tr->segments->length - 1)) && (j == (tmpSeg->waypoints->length - 1))){
                endLat = tmpWpt->latitude; 
                endLon = tmpWpt->longitude; 
            }
        }
    }
    if (wptCnt < 4){
        return false; 
    }
    float distance = haversine (startLat, startLon, endLat, endLon); 
    if (distance <= delta){
        return true; 
    }
    else {
        return false; 
    }   
}

/**
 * function checks to see if the source and destination (both!) exist
 * in the doc. i loop (could've done a while loop - didn't need it at this level) goes through arrays, j loop goes through waypoints
 * source is the first waypoint so (j == 0), destination is the final 
 * waypoint (so j == tmpRte->waypoints->length - 1)
 * if both source and destination are found within delta's tolerable range, 
 * add route to list, if not move onto the next route until they're all exhausted
 * CAN ALSO USE THE LIST API TO DO THIS WITH THE GETFROMBACK / FRONT FUNCTIONS 
 * -> e.g. void *elem = getFromFront (rte->waypoints) & getFromBack (rte->waypoints) 
 */

List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
    if (doc == NULL){
        return NULL;
    }

    List *routeList = initializeList (&routeToString, &dummyDelete, &compareRoutes); 

    int i = 0; 
    int j = 0; 

    ListIterator iter = createIterator (doc->routes); 
    void *elem ;

    for (i = 0; i < doc->routes->length; i++){
        elem = nextElement (&iter); 
        Route *tmpRte = (Route *)elem; 

        ListIterator iter2 = createIterator (tmpRte->waypoints); 
        void *elem2; 

        for (j = 0; j < tmpRte->waypoints->length; j++){
            elem2 = nextElement (&iter2); 
            Waypoint *tmpWpt = (Waypoint *)elem2; 

            if (j == 0){
                float distance = haversine (sourceLat, sourceLong, tmpWpt->latitude, tmpWpt->longitude); 
                if (distance > delta){ //move onto next route -> route start doesn't match source within delta 
                    break; 
                }
            }
            else if (j == ((tmpRte->waypoints->length) - 1)){
                float distance = haversine (destLat, destLong, tmpWpt->latitude, tmpWpt->longitude); 
                if (distance > delta){
                    break; 
                }
                else {
                    insertBack (routeList, tmpRte);
                }
            }
        }
    }

    if (routeList->length == 0){
        freeList (routeList);
        return NULL; 
    }
    else {
        return routeList; 
    }
}

/**
 * similar logic to above but here the source has to be the first waypoint in the first track segment 
 * (j == 0, i == 0), and the destination has to be the final waypoint in the final track segmenet 
 * (j == tmpseg->waypoints->length - 1, tmpTrk->segments->length -1) 
 */
List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
    if (doc == NULL){
        return NULL; 
    }

    List *trackList = initializeList (&trackToString, &dummyDelete, &compareRoutes); 

    int i = 0; 
    int j = 0; 

    ListIterator iter = createIterator (doc->tracks); 
    void *elem; 

    while ((elem = nextElement (&iter)) != NULL){
        Track *tmpTrk = (Track *)elem;

        ListIterator iter2 = createIterator (tmpTrk->segments); 
        void *elem2; 

        for (i = 0; i < tmpTrk->segments->length; i++){ //loops through segments -> check i == 0 and i == segments->length - 1
            elem2 = nextElement (&iter2); 
            TrackSegment *tmpSeg = (TrackSegment *)elem2; 

            ListIterator iter3 = createIterator (tmpSeg->waypoints); 
            void *elem3; 
            
            for (j = 0; j < tmpSeg->waypoints->length; j++){
                elem3 = nextElement (&iter3); 
                Waypoint *tmpWpt = (Waypoint *)elem3; 
                if (j == 0 && i == 0){
                    float distance = haversine (sourceLat, sourceLong, tmpWpt->latitude, tmpWpt->longitude); 
                    if (distance > delta){
                        break; 
                    }
                }
                else if ((j == (tmpSeg->waypoints->length -1)) && (i == (tmpTrk->segments->length - 1))){
                    float distance = haversine (destLat, destLong, tmpWpt->latitude, tmpWpt->longitude); 
                    if (distance > delta){
                        break; 
                    }
                    else {
                        insertBack (trackList, tmpTrk); 
                    }
                }
            }
        }
    }

    if (trackList->length == 0){
        freeList (trackList); 
        return NULL; 
    }
    else {
        return trackList; 
    }
}

char* routeToJSON(const Route *rt){
    if (rt == NULL){
        char *emptyReturn = (char *)malloc (sizeof (char) * 3);
        strcpy (emptyReturn, "{}");
        return emptyReturn; 
    }

    int finalSize = 10; //initialized to 10 because: '{' + ':' + ',' + ':' + ',' + ':' + ',' + ':' + '}' + '\0' 
    char nameLabel[7] = "\"name\"";
    finalSize += strlen (nameLabel); 
    char *rteName;
    if (strlen (rt->name) > 0){
        rteName = (char *) malloc (strlen(rt->name) + 3); //'"' + '"' + '\0'
        sprintf (rteName, "\"%s\"", rt->name); 
    }
    else {
        rteName = (char *) malloc (7 * (sizeof (char)));
        strcpy (rteName, "\"None\""); 
    }
    finalSize += strlen (rteName); 
    char numPtsLabel[12] = "\"numPoints\"";
    finalSize += strlen(numPtsLabel); 
    int numPts = rt->waypoints->length; 
    int cnt = 0; 
    int tmpNumSize = numPts; 
    if (numPts <= 0){
        finalSize++; 
    }
    else {
        while (tmpNumSize != 0){
            tmpNumSize = tmpNumSize / 10; 
            cnt++; 
        } 
        finalSize += cnt; 
    }
    char lenLabel[6] = "\"len\""; 
    finalSize += strlen(lenLabel); 
    float lenVal = getRouteLen (rt); 
    lenVal = round10(lenVal); 
    cnt = 0; 
    tmpNumSize = (int) lenVal; 
    if (lenVal <= 0){
        finalSize = finalSize + 3; 
    }
    else {
        while (tmpNumSize != 0){
            tmpNumSize = tmpNumSize/10; 
            cnt++; 
        }
        cnt = cnt + 2; //'.' + '0' 
        finalSize += cnt; 
    }
    char loopLabel[7] = "\"loop\""; 
    finalSize += strlen (loopLabel); 
    char *boolString; 
    if (isLoopRoute(rt, 10) == true){
        boolString = (char *)malloc((sizeof (char)) * 5); 
        strcpy (boolString, "true"); 
    }
    else {
        boolString = (char *)malloc((sizeof (char)) * 6); 
        strcpy (boolString, "false"); 
    }
    finalSize += strlen(boolString); 
    char *finalString = (char *)malloc (sizeof(char) * finalSize); 
    sprintf (finalString, "{%s:%s,%s:%d,%s:%.1f,%s:%s}", nameLabel, rteName, numPtsLabel, numPts, lenLabel, lenVal, loopLabel, boolString);
    free(rteName); 
    free (boolString); 
    return finalString; 
}

char* trackToJSON(const Track *tr){
    if (tr == NULL){
        char *emptyReturn = (char *)malloc ((sizeof(char)) * 3); //'{' + '}' + '\0' 
        strcpy (emptyReturn, "{}"); 
        return emptyReturn; 
    }

    int finalSize = 10; //'{' + ':' + ',' + ':' + ',' + ':' + ',' + ':' + '}' + '\0' 

    char nameLabel[7] = "\"name\"";
    finalSize += strlen (nameLabel); 

    char *trkName;
    if (strlen (tr->name) > 0){
        trkName = (char *) malloc (strlen(tr->name) + 3); //'"' + '"' + '\0'
        sprintf (trkName, "\"%s\"", tr->name); 
    }
    else {
        trkName = (char *) malloc (7 * (sizeof (char)));
        strcpy (trkName, "\"None\""); 
    }
    finalSize += strlen (trkName); 

    char numPtsLabel[12] = "\"numPoints\"";
    finalSize += strlen(numPtsLabel); 


    int numPts = 0; 
    ListIterator iter = createIterator (tr->segments); 
    void *elem;
    while ((elem = nextElement(&iter)) != NULL){
        TrackSegment *tmpSeg = (TrackSegment *)elem; 
        numPts = numPts + tmpSeg->waypoints->length;
    }

    int cnt = 0; 
    int tmpNumSize = numPts; 
    if (numPts <= 0){
        finalSize++; 
    }
    else {
        while (tmpNumSize != 0){
            tmpNumSize = tmpNumSize / 10; 
            cnt++; 
        } 
        finalSize += cnt; 
    }


    char lenLabel [6] = "\"len\""; 
    finalSize += strlen (lenLabel); 

    float lenVal = getTrackLen (tr); 
    lenVal = round10(lenVal);

    int digitCnt = (int)lenVal; 
    cnt = 0; 
    if (lenVal <= 0){
        finalSize = finalSize + 3; 
    }
    else {
        while (digitCnt != 0){
            digitCnt = digitCnt / 10; 
            cnt++;
        }
        cnt = cnt + 2; //'.' + '0' 
        finalSize += cnt; 
    }

    char loopLabel[7] = "\"loop\""; 
    finalSize += strlen (loopLabel); 

    char *boolVal; 
    if (isLoopTrack (tr, 10) == true){
        boolVal = (char *)malloc((sizeof (char)) * 5);
        strcpy (boolVal, "true"); 
    }
    else {
        boolVal = (char *)malloc ((sizeof (char)) * 6);
        strcpy (boolVal, "false");
    }
    finalSize += strlen (boolVal); 

    char *finalString = (char *)malloc ((sizeof (char)) * finalSize);
    sprintf (finalString, "{%s:%s,%s:%d,%s:%.1f,%s:%s}", nameLabel, trkName, numPtsLabel, numPts, lenLabel, lenVal, loopLabel, boolVal);

    free(trkName);
    free(boolVal); 

    return finalString; 
}

char* routeListToJSON(const List *list){
    if (list == NULL || list->length == 0){
        char *emptyReturn = (char *)malloc((sizeof (char)) * 3); 
        strcpy (emptyReturn, "[]"); 
        return emptyReturn; 
    }
    ListIterator iter = createIterator ((List*)list); 
    void *elem; 
    int size = list->length; 
    char **strings = (char **)malloc (sizeof(char *) * size); 

    for (int i = 0; i < size; i++){
        elem = nextElement (&iter); 
        Route *tmpRte = (Route *)elem; 
        strings[i] = routeToJSON(tmpRte); 
    }

    int finalSize = (size - 1) + 3; //commas = n - 1 -> where n is number of items added to list, +2 = '[' + ']' + '\0'

    for (int i = 0; i < size; i++){
        finalSize += strlen (strings[i]);
    }

    char *finalString = (char *)malloc ((sizeof (char)) * finalSize);
    strcpy (finalString, "["); 

    for (int i = 0; i < size; i++){
        if (i == (size - 1)){
            strcat (finalString, strings[i]); 
            strcat (finalString, "]\0"); //explicility done just to make it visible that this is where it terminates and stops adding 
        }
        else {
            strcat (finalString, strings[i]); 
            strcat (finalString, ",");
        }
    }

    for (int i = 0; i < size; i++){
        free(strings[i]);  
    }
    free (strings); 

    return finalString;
}


char* trackListToJSON(const List *list){
    if (list == NULL || list->length == 0){
        char *emptyReturn = (char *)malloc((sizeof (char)) * 3);
        strcpy (emptyReturn, "[]"); 
        return emptyReturn; 
    }
    ListIterator iter = createIterator ((List *)list); 
    void *elem;
    int size = list->length; 
    char **strings = (char **)malloc ((sizeof (char *)) * size); 
    
    for (int i = 0; i < size; i++){
        elem = nextElement (&iter); 
        Track *tmpTrk = (Track *)elem; 
        strings[i] = trackToJSON (tmpTrk); 
    }

    int finalSize = (size - 1) + 3; //commas + [] + '\0'

    for (int i = 0; i < size; i++){
        finalSize += strlen(strings[i]); 
    }
    char *finalString = (char *)malloc((sizeof (char)) * finalSize); 
    strcpy (finalString, "["); 

    for (int i = 0; i < size; i++){
        if (i == (size - 1)){
            strcat (finalString, strings[i]); 
            strcat (finalString, "]\0"); 
        }
        else {
            strcat (finalString, strings[i]); 
            strcat (finalString, ",");
        }
    }

    for (int i = 0; i < size; i++){
        free(strings[i]);
    }
    free (strings); 

    return finalString; 
}

char* GPXtoJSON(const GPXdoc* gpx){
    if (gpx == NULL){
        char *emptyReturn = (char *)malloc (sizeof(char) * 3); 
        strcpy (emptyReturn, "{}"); 
        return emptyReturn; 
    }
    int finalSize = 12; // '{'':' + ',' + ':' + ',' + ':' + ',' + ':' + ',' + ':' + '}' + '\0'

    char versionLabel[10] = "\"version\""; 
    finalSize += strlen (versionLabel); 

    double version = gpx->version;
    finalSize += 3; 
    
    char creatorLabel[10] = "\"creator\"";
    finalSize += strlen (creatorLabel); 
    
    char *creator = (char *) malloc (((sizeof (char)) * (strlen(gpx->creator))) + 3); 
    sprintf (creator, "\"%s\"", gpx->creator); 
    finalSize += strlen (creator); 

    char numWptLabel[15] = "\"numWaypoints\"";
    finalSize += strlen (numWptLabel); 

    int numWpt = getNumWaypoints (gpx); 
    int tmpCnt; 
    int cnt = 0; 
    tmpCnt = numWpt; 
    if (numWpt <= 0){
        finalSize++; 
    }
    else {
        while (tmpCnt != 0){
            tmpCnt = tmpCnt / 10; 
            cnt++;
        }
        finalSize += cnt; 
    }

    char numRoutesLabel[12] = "\"numRoutes\"";
    finalSize += strlen (numRoutesLabel); 

    int numRte = getNumRoutes (gpx); 
    tmpCnt = numRte; 
    cnt = 0; 
    if (numRte <= 0){
        finalSize++; 
    }
    else {
        while (tmpCnt != 0){
            tmpCnt = tmpCnt / 10; 
            cnt++; 
        }
        finalSize += cnt; 
    }

    char numTracksLabel[12] = "\"numTracks\"";
    finalSize += strlen (numTracksLabel); 

    int numTrack = getNumTracks (gpx); 
    tmpCnt = numTrack; 
    cnt = 0; 
    if (numTrack <= 0){
        finalSize++; 
    }
    else {
        while (tmpCnt != 0){
            tmpCnt = tmpCnt /10; 
            cnt++; 
        }
        finalSize += cnt; 
    }

    char *finalString = (char *)malloc ((sizeof(char)) * finalSize);
    sprintf (finalString, "{%s:%.1f,%s:%s,%s:%d,%s:%d,%s:%d}", versionLabel, version, creatorLabel, creator, numWptLabel, numWpt, numRoutesLabel, numRte, numTracksLabel, numTrack);

    free (creator); 

    return finalString;
}

void addWaypoint(Route *rt, Waypoint *pt){
    if (rt == NULL || pt == NULL){
        return; 
    }
    insertBack (rt->waypoints, pt); 

}
void addRoute(GPXdoc* doc, Route* rt) {
    if (doc == NULL || rt == NULL){
        return; 
    }
    insertBack (doc->routes, rt); 
}

GPXdoc* JSONtoGPX(const char* gpxString){
    if (gpxString == NULL){
        return NULL; 
    }
    
    char *string = (char *)malloc (((sizeof (char)) * strlen (gpxString)) + 1);
    strcpy (string, gpxString); 

    char *version; 
    char *creator; 

    char *token; 
    const char delim[6] = "{\":,}"; 

    int i = 0; 
    token = strtok (string, delim);
    while (token != NULL){
        if (i == 1){
            version = (char *)malloc(strlen(token) + 1); 
            strcpy (version, token); 
        }
        else if (i == 3){
            creator = (char *)malloc(strlen(token) + 1); 
            strcpy (creator, token); 
        }
        token = strtok (NULL, delim);
        i++ ; 
    }
    
    GPXdoc *newDoc = (GPXdoc *)malloc (sizeof(GPXdoc)); 
    strcpy (newDoc->namespace, "http://www.topografix.com/GPX/1/1");
    newDoc->version = strtod(version, NULL);
    newDoc->creator = (char *)malloc (((sizeof (char)) * strlen(creator)) + 1);
    strcpy (newDoc->creator, creator); 

    newDoc->waypoints = initializeList (&waypointToString, &deleteWaypoint, &compareWaypoints);
    newDoc->routes = initializeList (&routeToString, &deleteRoute, &compareRoutes); 
    newDoc->tracks = initializeList (&trackToString, &deleteTrack, &compareTracks); 

    free (creator); 
    free (version);
    free (string); 
    return newDoc; 
}

Waypoint* JSONtoWaypoint(const char* gpxString){
    if (gpxString == NULL){
        return NULL;
    }
    double lat; 
    double lon; 

    char *string = (char *)malloc (((sizeof (char)) * strlen (gpxString)) + 1); 
    strcpy (string, gpxString); 

    char *token; 
    const char delim[6] = "{\":,}"; 
    int i = 0; 

    token = strtok (string, delim);
    while (token != NULL){
        if (i == 1){
            lat = strtod (token, NULL);
        }
        else if (i == 3){
            lon = strtod (token, NULL); 
        }
        token = strtok (NULL, delim); 
        i++; 
    }
    
    Waypoint *newWpt = createWPT(); 
    newWpt->latitude = lat; 
    newWpt->longitude = lon; 

    return newWpt; 
}

Route* JSONtoRoute(const char* gpxString){
    if (gpxString == NULL){
        return NULL; 
    }

    char *string = (char *) malloc ((sizeof(char) * strlen(gpxString)) + 1); 
    strcpy (string, gpxString);

    char *name; 
    char *token; 
    const char delim[6] = "{\",:}";

    int i = 0; 
    token = strtok (string, delim); 
    while (token != NULL){
        if (i == 1){
            name = (char *)malloc (strlen(token) +1); 
            strcpy (name, token); 
        }
        i++;
        token = strtok (NULL, delim);
    }

    Route *newRoute = createRTE ();
    newRoute->name = (char *) realloc (newRoute->name, strlen (name) + 1); 
    strcpy (newRoute->name, name);

    free (name);
    free(string); 

    return newRoute; 
}

/*A3 Functions*/

char *otherToJSON (const GPXData *data){
    int finalSize = 6; // '{' + ':' + ',' + ':' + }' + '\0'

    char typeLabel [] = "\"type\""; 
    finalSize += strlen (typeLabel); 

    char *typeStr = (char *)malloc(strlen(data->name) + 3); 
    sprintf (typeStr, "\"%s\"", data->name); 
    finalSize += strlen (typeStr); 

    char valueLabel[] = "\"value\""; 
    finalSize += strlen (valueLabel); 

    char *valueStr = (char *)malloc(strlen (data->value) + 3); 
    sprintf (valueStr, "\"%s\"", data->value);
    finalSize += strlen (valueStr); 

    char *retStr = (char *)malloc(finalSize); 
    sprintf (retStr, "{%s:%s,%s:%s}", typeLabel, typeStr, valueLabel, valueStr); 

    free(typeStr); 
    free(valueStr); 

    return retStr; 
}

char *otherListToJSON (const List *list){
    ListIterator iter = createIterator ((List *)list); 
    void *elem; 

    int size = list->length; 
    char **strings = (char **)malloc((sizeof (char *)) * size); 

    int finalSize = (size - 1) + 3; //commas (don't need for last one), [] (array) + '\0' 

    for (int i = 0; i < size; i++){
        elem = nextElement (&iter); 
        GPXData *otherTmp = (GPXData *)elem; 
        strings[i] = otherToJSON (otherTmp); 
        finalSize += strlen (strings[i]);
    }

    char *retStr = (char *)malloc(finalSize); 
    strcpy (retStr, "["); 

    for (int i = 0; i < size; i++){
        strcat (retStr, strings[i]);
        if (i == (size - 1)){
            strcat (retStr, "]\0");
        }
        else {
            strcat (retStr, ","); 
        }
        free (strings[i]); 
    }
    free (strings); 

    return retStr;
}

/**char *createValid (char *fileName, char *gpxFormatFile){
    GPXdoc *newDoc = createValidGPXdoc (fileName, gpxFormatFile);
    char *retStr; 
    retStr = GPXtoJSON (newDoc); 
    deleteGPXdoc (newDoc); //this won't be used so free this 
    return retStr; //returning malloced string -> js has automatic garbage collection so memory will be freed at some point?
}**/

char *listTrackJSON (char *fileName){
    GPXdoc *newDoc = createGPXdoc (fileName); 
    char *retStr = trackListToJSON (newDoc->tracks);
    deleteGPXdoc (newDoc); 
    return retStr; 
}

char *listRouteJSON (char *fileName){
    GPXdoc *newDoc = createGPXdoc (fileName); 
    char *retStr = routeListToJSON (newDoc->routes); 
    deleteGPXdoc (newDoc); 
    return retStr; 
}

//val = 0 -> route, val = 1 -> track 
char *listOtherJSON (char *fileName, int val, int itemNum){
    ListIterator iter; 
    void *elem; 
    char *retStr = NULL; 
    GPXdoc *newDoc = createGPXdoc (fileName); 
    if (val == 0){ //routes
        iter = createIterator(newDoc->routes); 

        for (int i = 0; i < itemNum; i++){
            elem = nextElement(&iter); 
        }

        Route *tmpRte = (Route *)elem; 
        retStr = otherListToJSON(tmpRte->otherData);
    }
    else if (val == 1){//track 
        iter = createIterator (newDoc->tracks); 
        
        for (int i = 0; i < itemNum; i++){
            elem = nextElement (&iter); 
        }
        Track *tmpTrk = (Track *)elem; 
        retStr = otherListToJSON(tmpTrk->otherData); 
    }
    deleteGPXdoc (newDoc); 
    return retStr; 
}

int changeName (char *fileName, char *gpxFormatFile, int val, int itemNum, char *newName){
    ListIterator iter; 
    void *elem; 
    bool succeeded; 
    GPXdoc *newDoc = createValidGPXdoc (fileName, gpxFormatFile); 
    if (newDoc == NULL){
        return false; 
    }

    if (val == 0){ //routes 
        iter = createIterator (newDoc->routes); 
        for (int i = 0; i < itemNum; i++){
            elem = nextElement (&iter); 
        }
        Route *tmpRte = (Route *)elem; 
        tmpRte->name = (char *)realloc (tmpRte->name, strlen (newName) + 1); 
        strcpy (tmpRte->name, newName); 
        succeeded = writeGPXdoc (newDoc, fileName); 
    }
    else if (val == 1){ //tracks
        iter = createIterator (newDoc->tracks); 
        for (int i = 0; i < itemNum; i++){
            elem = nextElement (&iter); 
        }
        Track *tmpTrk = (Track *)elem; 
        tmpTrk->name = (char *)realloc (tmpTrk->name, strlen (newName) + 1); 
        strcpy (tmpTrk->name, newName); 
        succeeded = writeGPXdoc (newDoc, fileName); 
    }
    deleteGPXdoc (newDoc); 
    return succeeded; 
}

char* UpdatedGPXtoJSON(const GPXdoc* gpx, char *fileName){
    if (gpx == NULL){
        char *emptyReturn = (char *)malloc (sizeof(char) * 3); 
        strcpy (emptyReturn, "{}"); 
        return emptyReturn; 
    }
    int finalSize = 14; // '{' ':' + ',' + ':' + ',' + ':' + ',' + ':' + ',' + ':' + ',' + ':' + '}' + '\0'

	char name[7] = "\"name\""; 
	finalSize += strlen (name); 
	
	int size = strlen (fileName) + 3; 
	char file[size]; 
	sprintf (file, "\"%s\"", fileName); 
	finalSize += strlen (file); 

    char versionLabel[10] = "\"version\""; 
    finalSize += strlen (versionLabel); 

    double version = gpx->version;
    finalSize += 3; 
    
    char creatorLabel[10] = "\"creator\"";
    finalSize += strlen (creatorLabel); 
    
    char *creator = (char *) malloc (((sizeof (char)) * (strlen(gpx->creator))) + 3); 
    sprintf (creator, "\"%s\"", gpx->creator); 
    finalSize += strlen (creator); 

    char numWptLabel[15] = "\"numWaypoints\"";
    finalSize += strlen (numWptLabel); 

    int numWpt = getNumWaypoints (gpx); 
    int tmpCnt; 
    int cnt = 0; 
    tmpCnt = numWpt; 
    if (numWpt <= 0){
        finalSize++; 
    }
    else {
        while (tmpCnt != 0){
            tmpCnt = tmpCnt / 10; 
            cnt++;
        }
        finalSize += cnt; 
    }

    char numRoutesLabel[12] = "\"numRoutes\"";
    finalSize += strlen (numRoutesLabel); 

    int numRte = getNumRoutes (gpx); 
    tmpCnt = numRte; 
    cnt = 0; 
    if (numRte <= 0){
        finalSize++; 
    }
    else {
        while (tmpCnt != 0){
            tmpCnt = tmpCnt / 10; 
            cnt++; 
        }
        finalSize += cnt; 
    }

    char numTracksLabel[12] = "\"numTracks\"";
    finalSize += strlen (numTracksLabel); 

    int numTrack = getNumTracks (gpx); 
    tmpCnt = numTrack; 
    cnt = 0; 
    if (numTrack <= 0){
        finalSize++; 
    }
    else {
        while (tmpCnt != 0){
            tmpCnt = tmpCnt /10; 
            cnt++; 
        }
        finalSize += cnt; 
    }

    char *finalString = (char *)malloc ((sizeof(char)) * finalSize);
    sprintf (finalString, "{%s:%s,%s:%.1f,%s:%s,%s:%d,%s:%d,%s:%d}", name, file, versionLabel, version, creatorLabel, creator, numWptLabel, numWpt, numRoutesLabel, numRte, numTracksLabel, numTrack);

    free (creator); 

    return finalString;
}

char *readFiles (char *directory, char *gpxFormatFile){
	DIR *startPoint = opendir (directory); 
	struct dirent *file; 
	int cnt = 0; 
	
	if (startPoint == NULL){
		char *retString = (char *)malloc(3); 
		strcpy (retString, "[]\0"); 
		return retString; 
	}

	file = readdir (startPoint); 
	
	while (file != NULL){
		if ((strcmp (file->d_name, ".") != 0) && (strcmp (file->d_name, "..") != 0) && (strcmp (file->d_name, ".DS_Store") != 0)){
            cnt++; 
        }
		file = readdir (startPoint);
	}
	
	closedir (startPoint); 
	
	if (cnt == 0){
		char *retString = (char *)malloc(3); 
		strcpy (retString, "[]\0"); 
		return retString; 
	}
	
	char *validStrings[cnt]; 
	
	startPoint = opendir(directory); 
	
	file = readdir (startPoint); 
	
	int i = 0; 
	
	while (file != NULL){
		if ((strcmp (file->d_name, ".") != 0) && (strcmp (file->d_name, "..") != 0) && (strcmp (file->d_name, ".DS_Store") != 0)){

			int size = strlen(directory) + strlen(file->d_name) + 1;
			char validFile[size]; 
			sprintf (validFile,"%s%s", directory, file->d_name); 

			GPXdoc *tmp = createValidGPXdoc (validFile, gpxFormatFile); 

			if (tmp != NULL){
                int sucLen = 6; // '{' + ':' + ',' + ':' '}' + '\0'

                char name[] = "\"name\""; 
                sucLen += strlen(name); 

                char tmpJSON[strlen(file->d_name) + 3]; 
                sprintf (tmpJSON, "\"%s\"", file->d_name);
                sucLen += strlen (tmpJSON); 


                char valid[] = "\"valid\""; 
                sucLen += strlen (valid); 

                int isValid = 1; 
                sucLen++; //valid integer (1)

                validStrings[i] = (char *)malloc(sucLen); 
                sprintf (validStrings[i], "{%s:%s,%s:%d}", name, tmpJSON, valid, isValid); 

				deleteGPXdoc(tmp);
			} else {
                int failLen = 6; // '{' + ':' + ',' + ':' '}' + '\0'

                char name[] = "\"name\""; 
                failLen += strlen(name); 
                
                char tmpJSON[strlen(file->d_name) + 3]; 
                sprintf (tmpJSON, "\"%s\"", file->d_name);
                failLen += strlen (tmpJSON); 

                char valid[] = "\"valid\""; 
                failLen += strlen (valid); 
                int isValid = 0; 
                failLen++; //valid integer (0)

                validStrings[i] = (char *)malloc(failLen); 
                sprintf (validStrings[i], "{%s:%s,%s:%d}", name, tmpJSON, valid, isValid); 
            }

            i++;
		}
		file = readdir (startPoint);
	}
	closedir(startPoint);  
	
	int length = (i - 1) + 3; //commas + [ + ]  + '\0'
	
	
	for (int j = 0; j < i; j++){
		length += strlen (validStrings[j]); 
	}
	
	char *retString = (char *)malloc(length); 
	
	strcpy (retString, "["); 
	for (int j = 0; j < i; j++){
		strcat (retString, validStrings[j]); 
		if (j == (i - 1)){
			strcat (retString, "]\0"); 
		}
		else {
			strcat (retString, ","); 
		}
	}

    for (int j = 0; j < i; j++){
        free (validStrings[j]); 
    }
    
	return retString; 
}

//HARDCODED IN PATH HERE -> CHECK IF HE SAYS NOT TO IN NOTES 
char *GPXTableInfo (char *fileName, char *schema){
    char validityCheck [strlen (fileName) + strlen("./uploads/") + 1];
    sprintf (validityCheck, "./uploads/%s", fileName);

    GPXdoc *newDoc = createValidGPXdoc (validityCheck, schema); 
    if (newDoc == NULL){
        return NULL; 
    }
    int finalSize = 14; // '{' + ':' + ',' + ':' + ',' + ':' + ',' + ':' + ',' + ':' + ',' + ':' + '}' + '\0' 
    
    char nameLabel[] = "\"name\""; 
    finalSize += strlen (nameLabel); 

    char newName[strlen(fileName) + 3]; 
    sprintf (newName, "\"%s\"", fileName); 
    finalSize += strlen (newName); 

    char versionLabel[] = "\"version\""; 
    finalSize += strlen (versionLabel); 

    double version = newDoc->version; 
    char updatedDouble[10]; 
    sprintf (updatedDouble, "%.1f", version); 
    finalSize += strlen (updatedDouble);

    char creatorLabel[] = "\"creator\"";
    finalSize += strlen (creatorLabel); 

    char newCreator [strlen(newDoc->creator) + 3];
    sprintf (newCreator, "\"%s\"", newDoc->creator);
    finalSize += strlen (newCreator);

    char wptLabel[] = "\"wptNum\""; 
    finalSize += strlen (wptLabel); 

    int numWpt = newDoc->waypoints->length; 
    char stringWpt[15]; 
    sprintf (stringWpt, "\"%d\"", numWpt); 
    finalSize += strlen (stringWpt); 

    char rteLabel[] = "\"rteNum\""; 
    finalSize += strlen (rteLabel); 

    int numRte = getNumRoutes(newDoc); 
    char stringRte[15]; 
    sprintf (stringRte, "\"%d\"", numRte); 
    finalSize += strlen(stringRte); 

    char trkLabel[] = "\"trkNum\""; 
    finalSize += strlen (trkLabel); 

    int numTrk = 0;

    ListIterator iter = createIterator (newDoc->tracks); 
    void *elem = NULL; 

    while ((elem = nextElement(&iter)) != NULL ){
        Track *tmpTrk = (Track *)elem; 
        
        ListIterator iter2 = createIterator (tmpTrk->segments); 
        void *elem2; 
        while ((elem2 = nextElement(&iter2)) != NULL){
            TrackSegment *tmpSeg = (TrackSegment *)elem2; 
            numTrk += tmpSeg->waypoints->length; 
        }
    }

    char stringTrk[15];
    sprintf (stringTrk, "\"%d\"", numTrk); 
    finalSize += strlen (stringTrk); 

    char *retStr = (char *)malloc(finalSize); 
    sprintf (retStr, "{%s:%s,%s:%s,%s:%s,%s:%s,%s:%s,%s:%s}", nameLabel, newName, versionLabel, updatedDouble, creatorLabel, newCreator, wptLabel, stringWpt, rteLabel, stringRte, trkLabel, stringTrk); 
    
    deleteGPXdoc (newDoc); 
    return retStr; 
}

//1 = true, 0 = falsee 
int addingValidRoutes (char *fileName, char *schema){ //path in full path for both
    GPXdoc *test = createValidGPXdoc (fileName, schema);
    if (test == NULL){
        return 0; 
    }
    else {
        deleteGPXdoc(test); 
        return 1; 
    }

}