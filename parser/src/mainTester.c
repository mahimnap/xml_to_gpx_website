#include "GPXParser.h"

//including private api just for isolation testing 
#include "GPXHelpers.h" 

//'../uploads/simple.gpx' -> works 

int main (int argc, char *argv[]){
    /*GPXdoc *myDoc; 
    myDoc = createValidGPXdoc (argv[1], "gpx.xsd"); 
    deleteGPXdoc (myDoc); */
    /*char *ret = createValid ("../uploads/simple.gpx", "../gpx.xsd"); 
    printf ("'%s'\n",ret); */

    /*GPXdoc *myDoc = createValidGPXdoc ("../uploads/simple.gpx", "../gpx.xsd"); 
    int itemNum = 1; 

    ListIterator iter = createIterator (myDoc->routes); 
    void *elem; 

    for (int i = 0; i < itemNum; i++){
        elem = nextElement (&iter); 
    }
    Route *tmp = (Route *)elem; 
    printf ("Original Name: %s\n", tmp->name); 

    char newName[] = "some new route"; 
    tmp->name = (char *) realloc (tmp->name, strlen (newName) + 1); 
    strcpy (tmp->name, newName); 
    printf ("New Name: %s\n", tmp->name); 

    writeGPXdoc (myDoc, "../uploads/simple.gpx");

    deleteGPXdoc (myDoc); */
    //char *string = readFiles ("../uploads/", "../gpx.xsd"); 
   // printf ("\n\n"); 
    //printf ("%s\n", string); 
    return 0; 
}
    /*ListIterator iter = createIterator (myDoc->routes);
    void *elem; 
    elem = nextElement (&iter); 
    Route *tmp = (Route *)elem;
    printf ("This: %s\n", tmp->name);
    printf ("%f\n", getRouteLen (tmp));

    Track *tmp2 = getFromBack(myDoc->tracks); 
    printf ("%d\n", tmp2->segments->length); 

    printf ("%f\n", getTrackLen (tmp2));*/

    //getRouteLen (getFromBack (myDoc->routes)); 

    //myDoc->routes
    //void *elem = getFromBack (myDoc->routes); 
    //char *ret = routeToJSON ((Route *)elem);

    //void *elem = getFromBack (myDoc->tracks);
    //char *ret = trackToJSON ((Track *)elem);

    /*void *elem = getFromBack (myDoc->tracks); 
    char *ret = trackToJSON ((Track *)elem); */

    //char *ret = routeListToJSON (myDoc->routes);

    //char *ret = trackListToJSON (myDoc->tracks); 

    //char *ret = GPXtoJSON (myDoc); 
    
    
    /*Route *newRoute = createRTE (); 
    printf ("size of route list: %d\n", myDoc->routes->length); 
    addRoute (myDoc, newRoute);
    printf ("size of route list: %d\n", myDoc->routes->length); 
    */

    /*Route *tmpRte = createRTE(); 
    printf ("Size of waypoint list in root before: %d\n", tmpRte->waypoints->length);
    Waypoint *tmpWpt = createWPT(); 
    addWaypoint (tmpRte, tmpWpt); 
    printf ("Size of waypoint list in root after: %d\n", tmpRte->waypoints->length);
    void *elem = getFromBack (tmpRte->waypoints); 
    char *ret = waypointToString (elem); 
    printf ("%s\n", ret); */
    /*
    char *ret = (char *)malloc(36);
    strcpy (ret, "{\"version\":1.1,\"creator\":\"WebTool\"}");
    GPXdoc *tmp = JSONtoGPX (ret); 
    printf ("%s | %.1f | %s\n", tmp->namespace, tmp->version, tmp->creator);
    */

    /*char *ret = (char *)malloc (35); 
    strcpy (ret,"{\"lat\":43.537299,\"lon\":-80.218267}");
    Waypoint *tmp = JSONtoWaypoint (ret); 
    char *ret2 = waypointToString (tmp);
    printf ("%s\n", ret2); */

    /*
    char *ret = (char *) malloc(25);
    strcpy (ret, "{\"name\":\"Reynolds walk\"}");
    Route *tmp = JSONtoRoute (ret); 
    char *ret2 = routeToString (tmp); 
    printf ("%s\n", ret2); */

    //printf ("%.1f|%.1f|%.1f|%.1f\n", round10 (8.5242), round10 (3.21), round10 (34.99), round10 (50.1)); 

    
    /*Route *tmp = getFromBack (myDoc->routes); 
    printf ("Length of route: %.2f\n", getRouteLen (tmp));
    
    Track *tmp2 = getFromBack (myDoc->tracks);
    printf ("Length of track: %.2f\n", getTrackLen(tmp2));*/


    //printf ("This many routes with desired length: %d\n", numRoutesWithLength (myDoc, 4000, 300)); 

    //printf ("This many tracks with desired length: %d\n", numTracksWithLength (myDoc, 350, 20)); 
    
    /*Route *tmp = getFromBack (myDoc->routes); 
    Waypoint *front = getFromFront (tmp->waypoints); 
    Waypoint *back = getFromBack (tmp->waypoints); 
    if (isLoopRoute (tmp,10) == true){
        printf ("(%.6f, %.6f) vs (%.6f, %.6f)\n", front->latitude, front->longitude, back->latitude, back->longitude);
    }*/

    /*
    Track *tmp = getFromBack (myDoc->tracks); 
    if (isLoopTrack (tmp, 10) == true){
        printf ("True\n");
    }
    */

    /*printf ("Size of route list in doc before: %d\n", myDoc->routes->length);
    List *myList = getRoutesBetween (myDoc, 43.549239, -80.220105, 43.549242, -80.220107, 10);
    if (myList->length > 0){
        ListIterator iter = createIterator (myList);
        void *elem; 
        while ((elem = nextElement (&iter)) != NULL){
            Route *tmpRte = (Route *)elem; 
            char *string = routeToString (tmpRte); 
            printf ("%s\n", string); 
            free (string); 
        }
    }
    freeList(myList); 
    printf ("Size of route list in doc after: %d\n", myDoc->routes->length);
    */
