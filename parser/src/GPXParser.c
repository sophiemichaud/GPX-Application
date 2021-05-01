/*
    * Name: Sophie Michaud
    * Student ID: 0999546
*/

/* Some snipets of code for the following functions 
 * were used from the libXmlExample.c example file
 * and the structListDemo.c example file from the course material.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>
#include "LinkedListAPI.h"
#include "GPXParser.h"
#include "GPXHelperFunctions.h"

/** Function to create an GPX object based on the contents of an GPX file.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid GPXdoc has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pointer to the new struct or NULL
 *@param fileName - a string containing the name of the GPX file
**/

GPXdoc* createGPXdoc(char* fileName){

    /*if file name is empty string or NULL, createGPXdoc returns NULL */
    if(fileName == NULL || strcmp(fileName, "") == 0){
        return NULL;
    }

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(fileName, NULL, 0);

    /*if the xmlReadFile returns NULL (ie. unreadable) createGPXdoc returns NULL */
    if (doc == NULL) {
        return NULL;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);
    GPXdoc *gpx = malloc(sizeof(GPXdoc));
    gpx -> waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    gpx -> routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    gpx -> tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);
    createGPXdocHelper(root_element, gpx);

    /*free the document */
    xmlFreeDoc(doc);

    return gpx;
}

/** Function to create a string representation of an GPX object.
 *@pre GPX object exists, is not null, and is valid
 *@post GPX has not been modified in any way, and a string representing the GPX contents has been created
 *@return a string contaning a humanly readable representation of an GPX object
 *@param obj - a pointer to an GPX struct
**/
char* GPXdocToString(GPXdoc* doc){

    char* to_string = "";

    if(doc == NULL){
        return NULL;
    }

    int len = strlen(doc -> namespace) + 10;
    to_string = malloc(len*sizeof(char) + 1);

    //add namespace information to string
    strcpy(to_string, "Namespace: ");
    to_string = concatFunc(to_string, doc -> namespace);

    //add version information to string
    to_string = concatFunc(to_string, "\nVersion: ");
    char versiontoString[20];
    sprintf(versiontoString, "%f", doc -> version);
    to_string = concatFunc(to_string, versiontoString);

    //add creator information to string
    to_string = concatFunc(to_string, "\nCreator: ");
    to_string = concatFunc(to_string, doc -> creator);

    //add waypoint information to string
    to_string = concatFunc(to_string, "\n\nWaypoints: ");
    char *waypoints = toString(doc -> waypoints);
    to_string = concatFunc(to_string, waypoints);
    free(waypoints);

    //add route information to string
    to_string = concatFunc(to_string, "\n\nRoutes: ");
    char *routes = toString(doc -> routes);
    to_string = concatFunc(to_string, routes);
    free(routes);

    //add track information to string
    to_string = concatFunc(to_string, "\n\nTracks: ");
    char *tracks = toString(doc -> tracks);
    to_string = concatFunc(to_string, tracks);
    free(tracks);

    return to_string;
}

/** Function to delete doc content and free all the memory.
 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object had been freed
 *@return none
 *@param obj - a pointer to an GPX struct
**/
void deleteGPXdoc(GPXdoc* doc){

    xmlCleanupParser();

    if(doc != NULL){

        //frees all members of the GPXdoc struct
        free(doc -> creator);
        freeList(doc -> waypoints);
        freeList(doc -> routes);
        freeList(doc -> tracks);

        //frees the doc itself
        free(doc);
    }
}

/** For the five "get..." functions below, return the count of specified entities from the file.  
   They all share the same format and only differ in what they have to count.
 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object has not been modified in any way
 *@return the number of entities in the GPXdoc object
 *@param obj - a pointer to an GPXdoc struct
**/

//Total number of waypoints in the GPX file
int getNumWaypoints(const GPXdoc* doc){

    /*If the GPX object is NULL or has been freed, returns NULL */
    if(doc == NULL){
        return 0;
    }

    int numWaypoints = 0;
    numWaypoints = getLength(doc -> waypoints);

    return numWaypoints;
}

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc){

    /*If the GPX object is NULL or has been freed, returns NULL */
    if(doc == NULL){
        return 0;
    }

    int numRoutes = 0;
    numRoutes = getLength(doc -> routes);

    return numRoutes;
}

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc){

    /*If the GPX object is NULL or has been freed, returns NULL */
    if(doc == NULL){
        return 0;
    }

    int numTracks = 0;
    numTracks = getLength(doc -> tracks);

    return numTracks;
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc){

    /*If the GPX object is NULL or has been freed, returns NULL */
    if(doc == NULL){
        return 0;
    }

    int numSegments = 0;

    void *elem;
    ListIterator iterator = createIterator(doc -> tracks);

    //iterates through the tracks to determine the number of track segments
    while((elem = nextElement(&iterator)) != NULL){

        Track* trk = (Track*)elem;
        int numSegs = 0;

        if(getLength(trk -> segments) > 0){
            numSegs = getLength(trk -> segments);
            numSegments = numSegments + numSegs;
        }
    }

    return numSegments;
}

//Total number of GPXData elements in the document
int getNumGPXData(const GPXdoc* doc){

    /*If the GPX object is NULL or has been freed, returns NULL */
    if(doc == NULL){
        return 0;
    }

    int numGPXData = 0;
    void *elem;
    ListIterator iterator = createIterator(doc -> routes);

    while((elem = nextElement(&iterator)) != NULL){

        Route* rte = (Route*)elem;

        if(getLength(rte -> waypoints) > 0){
            ListIterator iter = createIterator(rte -> waypoints);
            void *elem2;

            while((elem2 = nextElement(&iter)) != NULL){
                Waypoint* way = (Waypoint*)elem2;

                if(strcmp(way -> name, "") != 0){
                    numGPXData = numGPXData + 1;
                }
            }
        }

        if(getLength(rte -> otherData) > 0){
            numGPXData = numGPXData + getLength(rte -> otherData);
        }

        if(strcmp(rte -> name, "") != 0){
            numGPXData = numGPXData + 1;
        }
    }

    iterator = createIterator(doc -> waypoints);
    while((elem = nextElement(&iterator)) != NULL){

        Waypoint* wpt = (Waypoint*)elem;

        if(getLength(wpt -> otherData) > 0){
            numGPXData = numGPXData + getLength(wpt -> otherData);
        }
        if(strcmp(wpt -> name, "") != 0){
            numGPXData = numGPXData + 1;
        }
    }

    iterator = createIterator(doc -> tracks);
    while((elem = nextElement(&iterator)) != NULL){

        Track* trk = (Track*)elem;

        if(strcmp(trk -> name, "") != 0){
            numGPXData = numGPXData + 1;
        }

        if(getLength(trk -> otherData) > 0){
            numGPXData = numGPXData + getLength(trk -> otherData);
        }
    
        if(getLength(trk -> segments) > 0){
            
            ListIterator iter2 = createIterator(trk -> segments);
            void *elem2;

            while((elem2 = nextElement(&iter2)) != NULL){

                TrackSegment *seg = (TrackSegment*)elem2;
                if(getLength(seg -> waypoints) > 0){
                    
                    ListIterator iter3 = createIterator(seg -> waypoints);
                    void *elem3;

                    while((elem3 = nextElement(&iter3)) != NULL){

                        Waypoint *wpt = (Waypoint*)elem3;

                        if(strcmp(wpt -> name, "") != 0){
                            numGPXData = numGPXData + 1;
                        }
                        if(getLength(wpt -> otherData) > 0){
                            numGPXData = numGPXData + getLength(wpt -> otherData);
                        }
                    }
                }
            }
        }
    }
    return numGPXData;
}

// Function that returns a waypoint with the given name.  If more than one exists, return the first one.  
// Return NULL if the waypoint does not exist
Waypoint* getWaypoint(const GPXdoc* doc, char* name){

    Waypoint* waypoint = NULL;

    //checks if any of the arguments are NULL
    if(doc == NULL || name == NULL){
        return NULL;
    }

    ListIterator iter = createIterator(doc -> waypoints);
    void *elem;

    //iterates through the waypoint list to determine if there is a matching element
    while((elem = nextElement(&iter)) != NULL){
        Waypoint *wpt = (Waypoint*)elem;

        if(strcmp(wpt -> name, name) == 0){
            waypoint = wpt;
        }
    }
    return waypoint;
}

// Function that returns a track with the given name.  If more than one exists, return the first one. 
// Return NULL if the track does not exist 
Track* getTrack(const GPXdoc* doc, char* name){

    Track* track = NULL;

    //checks if any of the arguments are NULL
    if(doc == NULL || name == NULL){
        return NULL;
    }

    ListIterator iter = createIterator(doc -> tracks);
    void *elem;

    //iterates to determine if there are any Track elements with the same name
    while((elem = nextElement(&iter)) != NULL){
        Track *trk = (Track*)elem;

        if(strcmp(trk -> name, name) == 0){
            track = trk;
        }
    }
    return track;
}

// Function that returns a route with the given name.  If more than one exists, return the first one.  
// Return NULL if the route does not exist
Route* getRoute(const GPXdoc* doc, char* name){

    Route* route = NULL;

    //checks if any of the arguments are NULL
    if(doc == NULL || name == NULL){
        return NULL;
    }

    ListIterator iter = createIterator(doc -> routes);
    void *elem;

    //iterates to determine if there are any Route elements with the same name
    while((elem = nextElement(&iter)) != NULL){
        Route *rte = (Route*)elem;

        if(strcmp(rte -> name, name) == 0){
            route = rte;
        }
    }
    return route;
}


/* ******************************* List helper functions  - MUST be implemented *************************** */

/* ******************************* GPXData Helper Functions *************************** */

void deleteGpxData(void* data){

    /*cast the input void pointer to a pointer of type GPXData */
    GPXData *gpxDataObject = (GPXData*) data;

    /*frees only the GPXData struct itself due to flexible array member*/
    free(gpxDataObject);
}

char* gpxDataToString( void* data){

    GPXData *new_data = (GPXData*)data;

    char* to_string = "";

    int len = strlen(new_data -> name) + 10;
    to_string = malloc(len*sizeof(char) + 1);

    //add name information to string
    strcpy(to_string, "\t\tName: ");
    to_string = concatFunc(to_string, new_data -> name);

    //add value information to string
    to_string = concatFunc(to_string, "\n\t\tValue: ");
    to_string = concatFunc(to_string, new_data -> value);

    return to_string;
}

int compareGpxData(const void *first, const void *second){

    //compare both objects
    if(first == second){
        return 0;
    }
    else if (first != second){
        return -1;
    }

    return 1;
}

/* ******************************* Waypoint Helper Functions *************************** */

void deleteWaypoint(void* data){

    /*cast the input void pointer to a pointer of type Waypoint */
    Waypoint *gpxWaypoint = (Waypoint*) data;

    /*frees the dynamically allocated Waypoint struct members (name, otherData) */
    if(strcmp(gpxWaypoint -> name, "") != 0){
        free(gpxWaypoint -> name);
    }
    freeList(gpxWaypoint -> otherData);

    /*frees the Waypoint struct itself */
    free(gpxWaypoint);
}

char* waypointToString(void* data){

    Waypoint *wpt = (Waypoint*)data;

    char* to_string = "";

    int len = strlen(wpt -> name) + 10;
    to_string = malloc(len*sizeof(char) + 1);

    //add name information to string
    strcpy(to_string, "\tName: ");
    to_string = concatFunc(to_string, wpt -> name);

    //add longitude information to string
    to_string = concatFunc(to_string, "\n\t\tLongitude: ");
    char longitudetoString[20];
    sprintf(longitudetoString, "%f", wpt -> longitude);
    to_string = concatFunc(to_string, longitudetoString);

    //add latitude information to string
    to_string = concatFunc(to_string, "\n\t\tLatitude: ");
    char latitudetoString[20];
    sprintf(latitudetoString, "%f", wpt -> latitude);
    to_string = concatFunc(to_string, latitudetoString);

    //add any other additional GPXdata to string if it exists
    if(getLength(wpt -> otherData) > 0){
        to_string = concatFunc(to_string, "\n\t\tGPXData: ");
        char *other = toString(wpt -> otherData);
        to_string = concatFunc(to_string, other);
        free(other);
    }

    return to_string;
}

int compareWaypoints(const void *first, const void *second){

    //compares both objects
    if(first == second){
        return 0;
    }
    else if (first != second){
        return -1;
    }

    return 1;
}

/* ******************************* Route Helper Functions *************************** */

void deleteRoute(void* data){

    /*cast the input void pointer to a pointer of type Route */
    Route *gpxRoute = (Route*) data;

    /*frees the dynamically allocated Route struct members (name, waypoints, otherData) */
    free(gpxRoute -> name);
    freeList(gpxRoute -> waypoints);
    freeList(gpxRoute -> otherData);

    /*frees the Route struct itself */
    free(gpxRoute);
}

char* routeToString(void* data){

    Route *rte = (Route*)data;

    char* to_string = "";

    int len = strlen(rte -> name) + 10;
    to_string = malloc(len*sizeof(char) + 1);

    //add name information to string
    strcpy(to_string, "\tName: ");
    to_string = concatFunc(to_string, rte -> name);

    //add any other additional waypoints to string if it exists
    if(getLength(rte -> waypoints) > 0){
        to_string = concatFunc(to_string, "\n\n\tWaypoints: ");
        char *waypoints = toString(rte -> waypoints);
        to_string = concatFunc(to_string, waypoints);
        free(waypoints);
    }

    //add any other additional GPXdata to string if it exists
    if(getLength(rte -> otherData) > 0){
        to_string = concatFunc(to_string, "\n\n\tGPXData: ");
        char *other = toString(rte -> otherData);
        to_string = concatFunc(to_string, other);
        free(other);
    }

    return to_string;
}

int compareRoutes(const void *first, const void *second){

    //compares both objects
    if(first == second){
        return 0;
    }
    else if (first != second){
        return -1;
    }

    return 1;
}

/* ******************************* TrackSegment Helper Functions *************************** */

void deleteTrackSegment(void* data){

    /*cast the input void pointer to a pointer of type TrackSegment */
    TrackSegment *gpxTrackSegment = (TrackSegment*) data;

    /*frees the dynamically allocated TrackSegment struct members (waypoints) */
    freeList(gpxTrackSegment -> waypoints);

    /*frees the TrackSegment struct itself */
    free(gpxTrackSegment);
}

char* trackSegmentToString(void* data){

    TrackSegment *seg = (TrackSegment*)data;
    char* to_string = "";
    to_string = malloc(sizeof(char) * 15);

    //add waypoint information to string
    if(getLength(seg -> waypoints) > 0){
        strcpy(to_string, "\n\t\tWaypoints: ");
        char *waypoints = toString(seg -> waypoints);
        to_string = concatFunc(to_string, waypoints);
        free(waypoints);
    }

    return to_string;
}

int compareTrackSegments(const void *first, const void *second){

    //compares both objects
    if(first == second){
        return 0;
    }
    else if (first != second){
        return -1;
    }

    return 1;
}

/* ******************************* Track Helper Functions *************************** */

void deleteTrack(void* data){

    /*cast the input void pointer to a pointer of type Track */
    Track *gpxTrack = (Track*) data;

    /*frees the dynamically allocated Track struct members (name, segments, otherData) */
    free(gpxTrack -> name);
    freeList(gpxTrack -> segments);
    freeList(gpxTrack -> otherData);

    /*frees the Track struct itself */
    free(gpxTrack);
}

char* trackToString(void* data){

    Track *trk = (Track*)data;

    char* to_string = "";

    int len = strlen(trk -> name) + 10;
    to_string = malloc(len*sizeof(char) + 1);

    //add name information to string
    strcpy(to_string, "\tName: ");
    to_string = concatFunc(to_string, trk -> name);

    //add any other additional waypoints to string if it exists
    if(getLength(trk -> segments) > 0){
        to_string = concatFunc(to_string, "\n\tSegments: ");
        char *segments = toString(trk -> segments);
        to_string = concatFunc(to_string, segments);
        free(segments);
    }

    //add any other additional GPXdata to string if it exists
    if(getLength(trk -> otherData) > 0){
        to_string = concatFunc(to_string, "\n\n\tGPXData: ");
        char *other = toString(trk -> otherData);
        to_string = concatFunc(to_string, other);
        free(other);
    }

    return to_string;
}

int compareTracks(const void *first, const void *second){

    //compares both objects
    if(first == second){
        return 0;
    }
    else if (first != second){
        return -1;
    }

    return 1;
}

/************************************** A2 **************************************/

/* Some snipets of code for A2 functions
 * were used from the following sources:
 *
 * http://www.xmlsoft.org/examples/tree2.c (creating an XML tree struct and saving an XML tree to a file)
 * http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html (Validating an XML tree against a schema file)
 * http://www.xmlsoft.org/html/libxml-tree.html (functions for navigating and creating XML tree elements)
 * http://www.xmlsoft.org/html/libxml-xmlschemastypes.html (functions for working with XML schema)
 */

/******************************* A2 - Module 1 ************************************/

/** Function to create an GPX object based on the contents of an GPX file.
 * This function must validate the XML tree generated by libxml against a GPX schema file
 * before attempting to traverse the tree and create an GPXdoc struct
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid GPXdoc has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pointer to the new struct or NULL
 *@param gpxSchemaFile - the name of a schema file
 *@param fileName - a string containing the name of the GPX file
**/
GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile){

    /*if file name is empty string or NULL, createValidGPXdoc returns NULL */
    if(fileName == NULL || strcmp(fileName, "") == 0){
        return NULL;
    }

    /*if gpxSchemaFile name is empty string or NULL, createValidGPXdoc returns NULL */
    if(gpxSchemaFile == NULL || strcmp(gpxSchemaFile, "") == 0){
        return NULL;
    }

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION;

    /*parse the file and get the DOM */
    doc = xmlReadFile(fileName, NULL, 0);

    /*if the xmlReadFile returns NULL (ie. unreadable) createGPXdoc returns NULL */
    if (doc == NULL) {
        return NULL;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);
    GPXdoc *gpx = malloc(sizeof(GPXdoc));
    gpx -> waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    gpx -> routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    gpx -> tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);

    createGPXdocHelper(root_element, gpx);

    if(!validateGPXDoc(gpx, gpxSchemaFile)){

        xmlFreeDoc(doc);
        deleteGPXdoc(gpx);
        xmlMemoryDump();
        return NULL;
    }

    /*free the document */
    xmlFreeDoc(doc);
    xmlMemoryDump();

    return gpx;
}

/** Function to validating an existing a GPXobject object against a GPX schema file
 *@pre 
    GPXdoc object exists and is not NULL
    schema file name is not NULL/empty, and represents a valid schema file
 *@post GPXdoc has not been modified in any way
 *@return the boolean aud indicating whether the GPXdoc is valid
 *@param doc - a pointer to a GPXdoc struct
 *@param gpxSchemaFile - the name of a schema file
 **/
bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile){

    bool isValid = true;

    //checks that GPXdoc object exists and it is not NULL
    if(doc == NULL){
        return false;
    }

    //checks that file name is not NULL/empty
    if(gpxSchemaFile == NULL || strcmp(gpxSchemaFile, "") == 0){
        return false;
    }

    LIBXML_TEST_VERSION;

    xmlDocPtr xmlDoc = convertGPXtoXml(doc);
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);


    if (xmlDoc == NULL){
        return false;
    }
    else{
    
        xmlSchemaValidCtxtPtr ctxt;
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, xmlDoc);
        if (ret == 0){
            isValid = true;
        }
        else if (ret > 0){
            xmlSchemaFree(schema);
            xmlSchemaFreeValidCtxt(ctxt);
            xmlFreeDoc(xmlDoc);
            return false;
        }
        else{
            xmlSchemaFree(schema);
            xmlSchemaFreeValidCtxt(ctxt);
            xmlFreeDoc(xmlDoc);
            return false;
        }
        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(xmlDoc);
    }

    // free the resource
    if(schema != NULL){
        xmlSchemaFree(schema);
    }

    xmlSchemaCleanupTypes();
    xmlMemoryDump();

    // Checks whether the GPXDoc violates any constraints in GPXParser.h
    if(validatesConstraints(doc) == false){
        return false;
    }

    return isValid;
}

/** Function to writing a GPXdoc into a file in GPX format.
 *@pre
    GPXdoc object exists, is valid, and and is not NULL.
    fileName is not NULL, has the correct extension
 *@post GPXdoc has not been modified in any way, and a file representing the
    GPXdoc contents in GPX format has been created
 *@return a boolean value indicating success or failure of the write
 *@param
    doc - a pointer to a GPXdoc struct
 	fileName - the name of the output file
 **/
bool writeGPXdoc(GPXdoc* doc, char* fileName){

    bool writeStatus = true;

    LIBXML_TEST_VERSION;

    // If GPXdoc struct is NULL, writeGPXdoc returns false
    if(doc == NULL){
        return false;
    }

    // If file name is empty string or NULL, writeGPXdoc returns false
    if(fileName == NULL || strcmp(fileName, "") == 0){
        return false;
    }

    xmlDocPtr xmlDoc = convertGPXtoXml(doc);

    xmlSaveFormatFileEnc(fileName, xmlDoc, "UTF-8", 1);

    // frees xml document
    xmlFreeDoc(xmlDoc);

    //this is to debug memory for regression tests
    xmlMemoryDump();

    return writeStatus;
}


/******************************* A2 - Module 2 ************************************/

/** Function that returns the length of a Route
 *@pre Route object exists, is not null, and has not been freed
 *@post Route object had been freed
 *@return length of the route in meters
 *@param rt - a pointer to a Route struct
**/
float getRouteLen(const Route *rt){

    float routeLen = 0;

    // Returns 0 if route is NULL
    if(rt == NULL){
        return 0;
    }

    if(getLength(rt -> waypoints) > 0){

        int len = getLength(rt -> waypoints);
        double longitudes [len];
        double latitudes [len];
        ListIterator iter = createIterator(rt -> waypoints);
        void *elem;
        int i = 0;

        while((elem = nextElement(&iter)) != NULL){
            Waypoint *wpt = (Waypoint*)elem;
            longitudes[i] = wpt -> longitude;
            latitudes[i] = wpt -> latitude;
            i++;
        }

        for(int j = 0; j < len - 1; j++){
            routeLen = routeLen + computeHaversine(latitudes[j], latitudes[j+1], longitudes[j], longitudes[j+1]);
        }
    }

    return routeLen;
}

/** Function that returns the length of a Track
 *@pre Track object exists, is not null, and has not been freed
 *@post Track object had been freed
 *@return length of the track in meters
 *@param tr - a pointer to a Track struct
**/
float getTrackLen(const Track *tr){

    float trackLen = 0;

    // Returns 0 if track is NULL
    if(tr == NULL){
        return 0;
    }

    if(getLength(tr -> segments) > 0){

        ListIterator iter2 = createIterator(tr -> segments);
        int len2 = getLength(tr -> segments);
        void *elem2;
        int j = 0;
        double longitudesFirstLast [2];
        double latitudesFirstLast [2];

        while((elem2 = nextElement(&iter2)) != NULL){

            TrackSegment *seg = (TrackSegment*)elem2;
            int len = getLength(seg -> waypoints);
            ListIterator iter = createIterator(seg -> waypoints);
            void *elem;
            int i = 0;
            double longitudes [len];
            double latitudes [len];
        
            while((elem = nextElement(&iter)) != NULL){
                Waypoint *wpt = (Waypoint*)elem;
                longitudes[i] = wpt -> longitude;
                latitudes[i] = wpt -> latitude;
                i++;
            }

            for(int j = 0; j < len - 1; j++){
                trackLen = trackLen + computeHaversine(latitudes[j], latitudes[j+1], longitudes[j], longitudes[j+1]);
            }

            if(j == 0){
                latitudesFirstLast[0] = latitudes[len - 1];
                longitudesFirstLast[0] = longitudes[len - 1];
            }

            if(j == len2 - 1){
                latitudesFirstLast[1] = latitudes[0];
                longitudesFirstLast[1] = longitudes[0];
            }

            j++;

        }

        if(getLength(tr -> segments) > 1){
            trackLen = trackLen + computeHaversine(latitudesFirstLast[0], latitudesFirstLast[1], longitudesFirstLast[0], longitudesFirstLast[1]);
        }
    }

    return trackLen;
}

/** Function that rounds the length of a track or a route to the nearest 10m
 *@pre Length is not negative
  *@return length rounded to the nearest 10m
 *@param len - length
**/
float round10(float len){

    float rounded = 0;

    if(len < 0){
        return 0;
    }

    int a = floor(len / 10) * 10;
    int b = a + 10;

    if (len - a >= b - len){
        rounded = b;
    }
    else{
        rounded = a;
    }

    return rounded;
}

/** Function that returns the number routes with the specified length, using the provided tolerance 
 * to compare route lengths
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return the number of routes with the specified length
 *@param doc - a pointer to a GPXdoc struct
 *@param len - search route length
 *@param delta - the tolerance used for comparing route lengths
**/
int numRoutesWithLength(const GPXdoc* doc, float len, float delta){

    int numRoutes = 0;
    float max = len + delta;
    float min = len - delta;

    // Returns 0 if doc is NULL, or if len or delta are negative
    if(doc == NULL || len < 0 || delta < 0){
        return 0;
    }

    if(getLength(doc -> routes) > 0){
        
        ListIterator iter = createIterator(doc -> routes);
        void *elem;
        float routeLen = 0;

        while((elem = nextElement(&iter)) != NULL){
            Route *rte = (Route*)elem;
            routeLen = getRouteLen(rte);

            if(routeLen >= min && routeLen <= max){
                numRoutes = numRoutes + 1;
            }
        }
    }

    return numRoutes;
}


/** Function that returns the number tracks with the specified length, using the provided tolerance 
 * to compare track lengths
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return the number of tracks with the specified length
 *@param doc - a pointer to a GPXdoc struct
 *@param len - search track length
 *@param delta - the tolerance used for comparing track lengths
**/
int numTracksWithLength(const GPXdoc* doc, float len, float delta){

    int numTracks = 0;
    float max = len + delta;
    float min = len - delta;

    // Returns 0 if doc is NULL, or if len or delta are negative
    if(doc == NULL || len < 0 || delta < 0){
        return 0;
    }

    if(getLength(doc -> tracks) > 0){
        
        ListIterator iter = createIterator(doc -> tracks);
        void *elem;
        float trackLen = 0;

        while((elem = nextElement(&iter)) != NULL){
            Track *trk = (Track*)elem;
            trackLen = getTrackLen(trk);

            if(trackLen >= min && trackLen <= max){
                numTracks = numTracks + 1;
            }
        }
    }

    return numTracks;
}

/** Function that checks if the current route is a loop
 *@pre Route object exists, is not null
 *@post Route object exists, is not null, has not been modified
 *@return true if the route is a loop, false otherwise
 *@param route - a pointer to a Route struct
 *@param delta - the tolerance used for comparing distances between start and end points
**/
bool isLoopRoute(const Route* route, float delta){

    bool isRouteLoop = true;

    // Returns false if the Route is NULL or delta is negative
    if(route == NULL || delta < 0){
        return false;
    }

    // Checks that route has at least four waypoints
    if(getLength(route -> waypoints) < 4){
        return false;
    }

    // Checks that distance betwen first and last waypoints is less than delta
    int len = getLength(route -> waypoints);
    double longitudes [2];
    double latitudes [2];
    ListIterator iter = createIterator(route -> waypoints);
    void *elem;
    int i = 0;

    while((elem = nextElement(&iter)) != NULL){
        Waypoint *wpt = (Waypoint*)elem;
        if(i == 0){
            longitudes[0] = wpt -> longitude;
            latitudes[0] = wpt -> latitude;
        }
        if(i == len - 1){
            longitudes[1] = wpt -> longitude;
            latitudes[1] = wpt -> latitude;
        }
        i++;
    }

    float lenFirstLast = computeHaversine(latitudes[0], latitudes[1], longitudes[0], longitudes[1]);

    if(lenFirstLast <= delta){
        return true;
    }
    else{
        return false;
    }

    return isRouteLoop;
}


/** Function that checks if the current track is a loop
 *@pre Track object exists, is not null
 *@post Track object exists, is not null, has not been modified
 *@return true if the track is a loop, false otherwise
 *@param track - a pointer to a Track struct
 *@param delta - the tolerance used for comparing distances between start and end points
**/
bool isLoopTrack(const Track *tr, float delta){

    bool isTrackLoop = false;

    // Returns false if the Track is NULL or delta is negative
    if(tr == NULL || delta < 0){
        return false;
    }

    int numWpts = 0;

    if(getLength(tr -> segments) > 0){

        ListIterator iter2 = createIterator(tr -> segments);
        int len2 = getLength(tr -> segments);
        void *elem2;
        int j = 0;
        double longitudesFirstLast [2];
        double latitudesFirstLast [2];

        while((elem2 = nextElement(&iter2)) != NULL){

            TrackSegment *seg = (TrackSegment*)elem2;
            int len = getLength(seg -> waypoints);
            ListIterator iter = createIterator(seg -> waypoints);
            void *elem;
            int i = 0;
            double longitudes [len];
            double latitudes [len];
        
            while((elem = nextElement(&iter)) != NULL){
                Waypoint *wpt = (Waypoint*)elem;
                longitudes[i] = wpt -> longitude;
                latitudes[i] = wpt -> latitude;
                numWpts++;
                i++;
            }

            if(j == 0){
                latitudesFirstLast[0] = latitudes[len - 1];
                longitudesFirstLast[0] = longitudes[len - 1];
            }

            if(j == len2 - 1){
                latitudesFirstLast[1] = latitudes[0];
                longitudesFirstLast[1] = longitudes[0];
            }

            j++;

        }

        float lenFirstLast = computeHaversine(latitudesFirstLast[0], latitudesFirstLast[1], longitudesFirstLast[0], longitudesFirstLast[1]);

        if(lenFirstLast <= delta){
            return true;
        }
        else{
            return false;
        }

    }

    if(numWpts < 4){
        return false;
    }

    return isTrackLoop;
}


/** Function that returns all routes between the specified start and end locations
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return a list of Route structs that connect the given sets of coordinates
 *@param doc - a pointer to a GPXdoc struct
 *@param sourceLat - latitude of the start location
 *@param sourceLong - longitude of the start location
 *@param destLat - latitude of the destination location
 *@param destLong - longitude of the destination location
 *@param delta - the tolerance used for comparing distances between waypoints 
*/
List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){

    List *routesBetween = NULL;

    if(doc == NULL || delta < 0){
        return NULL;
    }

    if(getLength(doc -> routes) > 0){

        ListIterator iter = createIterator(doc -> routes);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){

            Route *rte = (Route*)elem;
            Waypoint *first = getFromFront(rte -> waypoints);
            Waypoint *last = getFromBack(rte -> waypoints);

            float sourceToFirst = computeHaversine(sourceLat, first -> latitude, sourceLong, first -> longitude);
            float destToLast = computeHaversine(destLat, last -> latitude, destLong, last -> longitude);
            float sourceToDest = computeHaversine(sourceLat, destLat, sourceLong, destLong);

            if(sourceToFirst + destToLast <= sourceToDest + delta){

                if(routesBetween == NULL){
                    routesBetween = initializeList(&routeToString, &deleteRouteDummy, &compareRoutes);
                }

                insertBack(routesBetween, rte);
            }
        }
    }
    return routesBetween;
}

/** Function that returns all Tracks between the specified start and end locations
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return a list of Track structs that connect the given sets of coordinates
 *@param doc - a pointer to a GPXdoc struct
 *@param sourceLat - latitude of the start location
 *@param sourceLong - longitude of the start location
 *@param destLat - latitude of the destination location
 *@param destLong - longitude of the destination location
 *@param delta - the tolerance used for comparing distances between waypoints 
*/
List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){

    List *tracksBetween = NULL;

    if(doc == NULL || delta < 0){
        return NULL;
    }

    if(getLength(doc -> routes) > 0){

        ListIterator iter2 = createIterator(doc -> tracks);
        void *elem2;
        float sourceToFirst = 0;
        float sourceToDest = 0;
        float destToLast = 0;

        while((elem2 = nextElement(&iter2)) != NULL){

            Track *trk = (Track*)elem2;

            ListIterator iter = createIterator(trk -> segments);
            void *elem;

            while((elem = nextElement(&iter)) != NULL){

                TrackSegment *seg = (TrackSegment*)elem;
                Waypoint *first = getFromFront(seg -> waypoints);
                Waypoint *last = getFromBack(seg -> waypoints);

                sourceToFirst = computeHaversine(sourceLat, first -> latitude, sourceLong, first -> longitude);
                destToLast = computeHaversine(destLat, last -> latitude, destLong, last -> longitude);
                sourceToDest = computeHaversine(sourceLat, destLat, sourceLong, destLong);

            }

            if(sourceToFirst + destToLast <= sourceToDest + delta){

                if(tracksBetween == NULL){
                    tracksBetween = initializeList(&trackToString, &deleteTrackDummy, &compareTracks);
                }

                insertBack(tracksBetween, trk);
            }
        }
    }

    return tracksBetween;
}

/******************************* A2 - Module 3 ************************************/

/** Function to converting a Track into a JSON string
 *@pre Track is not NULL
 *@post Track has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a Track struct
 **/
char* trackToJSON(const Track *tr){

    if(tr == NULL){
        return NULL;
    }

    char *json = "";

    int len = 10;
    json = malloc(len*sizeof(char) + 1);

    // Sets the name
    strcpy(json, "{\"name\":\"");
    if(strcmp(tr -> name, "") == 0){
        json = concatFunc(json, "\"None\"\",");
    }
    else{
        json = concatFunc(json, tr -> name);
        json = concatFunc(json, "\",");
    }

    int numPoints = 0;

    if(tr -> segments > 0){
        ListIterator iter = createIterator(tr -> segments);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){

            TrackSegment *seg = (TrackSegment*)elem;

            ListIterator iter2 = createIterator(seg -> waypoints);
            void *elem2;

            while((elem2 = nextElement(&iter2)) != NULL){
                numPoints++;
            }
        }
    }

    // Sets the number of waypoints
    json = concatFunc(json, "\"numPoints\":");
    char numtoString[20];
    sprintf(numtoString, "%d", numPoints);
    json = concatFunc(json, numtoString);

    // Sets the route length
    json = concatFunc(json,",\"len\":");
    char lentoString[20];
    sprintf(lentoString, "%.1f", round10(getTrackLen(tr)));
    json = concatFunc(json, lentoString);

    // Sets the loop status
    json = concatFunc(json, ",\"loop\":");
    if(isLoopTrack(tr, 10)){
        json = concatFunc(json, "true}");
    }
    else{
        json = concatFunc(json, "false}");
    }

    return json;
}

/** Function to converting a Route into a JSON string
 *@pre Route is not NULL
 *@post Route has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a Route struct
 **/

/** The format for the JSON string is: 
 *{"name":"routeName","numPoints":numVal,"len":routeLen,"loop":loopStat} 
 **/ 
char* routeToJSON(const Route *rt){

    if(rt == NULL){
        return NULL;
    }

    char *json = "";

    int len = 10;
    json = malloc(len*sizeof(char) + 1);

    // Sets the name
    strcpy(json, "{\"name\":\"");
    if(strcmp(rt -> name, "") == 0){
        json = concatFunc(json, "\"None\"\",");
    }
    else{
        json = concatFunc(json, rt -> name);
        json = concatFunc(json, "\",");
    }

    // Sets the number of waypoints
    json = concatFunc(json, "\"numPoints\":");
    char numtoString[20];
    sprintf(numtoString, "%d", getLength(rt -> waypoints));
    json = concatFunc(json, numtoString);

    // Sets the route length
    json = concatFunc(json,",\"len\":");
    char lentoString[20];
    sprintf(lentoString, "%.1f", round10(getRouteLen(rt)));
    json = concatFunc(json, lentoString);

    // Sets the loop status
    json = concatFunc(json, ",\"loop\":");
    if(isLoopRoute(rt, 10)){
        json = concatFunc(json, "true}");
    }
    else{
        json = concatFunc(json, "false}");
    }

    return json;
}

char* gpxDataToJSON(const GPXData *data){

    if(data == NULL){
        return NULL;
    }

    char *json = "";

    int len = 10;
    json = malloc(len*sizeof(char) + 1);

    // Sets the name
    strcpy(json, "{\"name\":\"");
    if(strcmp((char*)data -> name, "") == 0){
        json = concatFunc(json, "\"None\"\",");
    }
    else{
        json = concatFunc(json, (char*)data -> name);
        json = concatFunc(json, "\",");
    }

    // Sets the value
    json = concatFunc(json, "\"value\":\"");
    if(strcmp((char*)data -> value, "") == 0){
        json = concatFunc(json, "\"None\"\",");
    }
    else{
        json = concatFunc(json, (char*)data -> value);
        json = concatFunc(json, "\"}");
    }
    
    return json;
}

/** Function to converting a list of Route structs into a JSON string
 *@pre Route list is not NULL
 *@post Route list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char* routeListToJSON(const List *list){

    if(list == NULL){
        return NULL;
    }

    char *json = "";

    int len = 2;
    json = malloc(len*sizeof(char) + 1);
    strcpy(json, "[");
        
    ListIterator iter = createIterator((List*)list);
    void *elem;
    int i = 0;

    while((elem = nextElement(&iter)) != NULL){
        Route *rte = (Route*)elem;
        char *rteJson = routeToJSON(rte);
        json = concatFunc(json, rteJson);
        free(rteJson);

        if(i < getLength((List*)list) - 1){
            json = concatFunc(json, ",");
        }
        i++;
    }

    json = concatFunc(json, "]");

    return json;
}

char* dataListToJSON(const List *list){

    if(list == NULL){
        return NULL;
    }

    char *json = "";

    int len = 2;
    json = malloc(len*sizeof(char) + 1);
    strcpy(json, "[");
        
    ListIterator iter = createIterator((List*)list);
    void *elem;
    int i = 0;

    while((elem = nextElement(&iter)) != NULL){
        GPXData *data = (GPXData*)elem;
        char *dataJson = gpxDataToJSON(data);
        json = concatFunc(json, dataJson);
        free(dataJson);

        if(i < getLength((List*)list) - 1){
            json = concatFunc(json, ",");
        }
        i++;
    }

    json = concatFunc(json, "]");

    return json;
}

/** Function to converting a list of Track structs into a JSON string
 *@pre Track list is not NULL
 *@post Track list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char* trackListToJSON(const List *list){

    if(list == NULL){
        return NULL;
    }

    char *json = "";

    int len = 4;
    json = malloc(len*sizeof(char) + 1);
    strcpy(json, "[");
        
    ListIterator iter = createIterator((List*)list);
    void *elem;
    int i = 0;

    while((elem = nextElement(&iter)) != NULL){
        Track *trk = (Track*)elem;
        char *trkjson = trackToJSON(trk);
        json = concatFunc(json, trkjson);
        free(trkjson);

        if(i < getLength((List*)list) - 1){
            json = concatFunc(json, ",");
        }
        i++;
    }

    json = concatFunc(json, "]");

    return json;
}

/** Function to converting a GPXdoc into a JSON string
 *@pre GPXdoc is not NULL
 *@post GPXdoc has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a GPXdoc struct
 **/
char* GPXtoJSON(const GPXdoc* gpx){

    if(gpx == NULL){
        return NULL;
    }

    char *json = "";

    int len = 15;
    json = malloc(len*sizeof(char) + 1);

    // Sets version
    strcpy(json, "{\"version\":");
    char versiontoString[20];
    sprintf(versiontoString, "%.1f", gpx -> version);
    json = concatFunc(json, versiontoString);

    // Sets creator
    json = concatFunc(json, ",\"creator\":\"");
    json = concatFunc(json, gpx -> creator);

    // Sets waypoints
    json = concatFunc(json, "\",\"numWaypoints\":");
    char wptstoString[20];
    sprintf(wptstoString, "%d", getLength(gpx -> waypoints));
    json = concatFunc(json, wptstoString);

    // Sets routes
    json = concatFunc(json, ",\"numRoutes\":");
    char rtestoString[20];
    sprintf(rtestoString, "%d", getLength(gpx -> routes));
    json = concatFunc(json, rtestoString);

    // Sets tracks
    json = concatFunc(json, ",\"numTracks\":");
    char trkstoString[20];
    sprintf(trkstoString, "%d", getLength(gpx -> tracks));
    json = concatFunc(json, trkstoString);
    json = concatFunc(json, "}");

    return json;
}

/******************************* A2 - Bonus Functions ************************************/

/** Function to adding an Waypont struct to an existing Route struct
 *@pre arguments are not NULL
 *@post The new waypoint has been added to the Route's waypoint list
 *@return N/A
 *@param rt - a Route struct
 *@param pr - a Waypoint struct
 **/
void addWaypoint(Route *rt, Waypoint *pt){

    if(rt != NULL || pt != NULL){

        insertBack(rt -> waypoints, pt);
    }
}

/** Function to adding an Route struct to an existing GPXdoc struct
 *@pre arguments are not NULL
 *@post The new route has been added to the GPXdoc's routes list
 *@return N/A
 *@param doc - a GPXdoc struct
 *@param rt - a Route struct
 **/
void addRoute(GPXdoc* doc, Route* rt){

    if(doc != NULL || rt != NULL){

        insertBack(doc -> routes, rt);
    }
}

/** Function to converting a JSON string into an GPXdoc struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized GPXdoc struct
 *@param str - a pointer to a string
 **/
GPXdoc* JSONtoGPX(const char* gpxString){

    // JSON string is in format:
    // {"version":ver,"creator":"creatorValue"}

    if(gpxString == NULL || strcmp(gpxString, "") == 0){
        return NULL;
    }

    GPXdoc *doc = malloc(sizeof(GPXdoc));
    strcpy(doc -> namespace, "http://www.topografix.com/GPX/1/1");
    doc -> version = 0;
    doc -> routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    doc -> tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);
    doc -> waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);

    char str[strlen(gpxString) + 1];
    strcpy(str, gpxString);
    const char s[3] = ",\":";
    char *token;
    
    /* get the first token */
    token = strtok(str, s);
    int i = 0;
    
    /* walk through other tokens */
    while( token != NULL ) {

        if(strcmp(token , "{") != 0 && strcmp(token , "}") != 0 && strcmp(token , "version") != 0 && strcmp(token , "creator") != 0){
            if(i == 0){

                doc -> version = strtof(token, NULL);
                i++;
            }
            if(i == 1){
                doc -> creator = malloc(sizeof(char) * strlen(token) + 1);
                strcpy(doc -> creator, token);
            }
        }

        token = strtok(NULL, s);
    }
  
    return doc;
}

/** Function to converting a JSON string into an Waypoint struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized Waypoint struct
 *@param str - a pointer to a string
 **/
Waypoint* JSONtoWaypoint(const char* gpxString){

    // JSON string is in format:
    // {"lat":latVal,"lon":lonVal}

    if(gpxString == NULL || strcmp(gpxString, "") == 0){
        return NULL;
    }

    Waypoint *wpt = malloc(sizeof(Waypoint));
    wpt -> name = "";
    wpt -> otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    wpt -> latitude = 0;
    wpt -> longitude = 0;

    char str[strlen(gpxString) + 1];
    strcpy(str, gpxString);
    const char s[4] = ",\":}";
    char *token;
    
    /* get the first token */
    token = strtok(str, s);
    int i = 0;
    
    /* walk through other tokens */
    while( token != NULL ) {

        if(strcmp(token , "{") != 0 && strcmp(token , "}") != 0 && strcmp(token , "lat") != 0 && strcmp(token , "lon") != 0){

            if(i == 0){
                wpt -> latitude = strtof(token, NULL);
                i++;
            }
            if(i == 1){
                wpt -> longitude = strtof(token, NULL);
            }
        }

        token = strtok(NULL, s);
    }

    return wpt;
}

/** Function to converting a JSON string into an Route struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized Route struct
 *@param str - a pointer to a string
 **/
Route* JSONtoRoute(const char* gpxString){

    if(gpxString == NULL || strcmp(gpxString, "") == 0){
        return NULL;
    }

    Route *rte = malloc(sizeof(Route));
    rte -> waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    rte -> otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    char str[strlen(gpxString) + 1];
    strcpy(str, gpxString);
    const char s[4] = ",\":}";
    char *token;
    
    /* get the first token */
    token = strtok(str, s);
    int i = 0;
    
    /* walk through other tokens */
    while( token != NULL ) {

        if(strcmp(token , "{") != 0 && strcmp(token , "}") != 0 && strcmp(token , "name") != 0){

            if(i == 0){
                rte -> name = malloc(sizeof(char) * strlen(token) + 1);
                strcpy(rte -> name, token);
            }
        }

        token = strtok(NULL, s);
    }

    return rte;
}
