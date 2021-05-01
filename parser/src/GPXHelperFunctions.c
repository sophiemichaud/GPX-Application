/*
    * Name: Sophie Michaud
    * Student ID: 0999546
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

/**************************************** A1 Functions *******************************************/

/** Helper Function to create an GPX object based on the contents of an GPX file.
 *@pre 
 *@post 
 *@return the pointer to the new struct
 *@param 
**/
GPXdoc *createGPXdocHelper(xmlNode *root, GPXdoc *doc){

    xmlNode *cur_node = NULL;
    
    for (cur_node = root; cur_node != NULL; cur_node = cur_node->next) {

        if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name, "gpx") == 0){
            setGPXDocProperties(cur_node, doc);
        }
        if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name, "wpt") == 0){
            setWaypointProperties(cur_node, doc -> waypoints);
        }    
        if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name, "rte") == 0){
            setRouteProperties(cur_node, doc);
        }
        if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name, "trk") == 0){
            setTrackProperties(cur_node, doc);
        }
        createGPXdocHelper(cur_node -> children, doc);
    }
    return doc;
}

void setGPXDocProperties(xmlNode *cur_node, GPXdoc *doc){

    strncpy(doc -> namespace, (const char *)cur_node -> ns -> href, sizeof(doc -> namespace));
       
    xmlAttr *attr;
    for (attr = cur_node->properties; attr != NULL; attr = attr->next){

        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);

        if(strcmp(attrName, "version") == 0){
            char *ptr;
            doc -> version = strtod(cont, &ptr);
        }
        else if(strcmp(attrName, "creator") == 0){
            int len = strlen(cont);
            doc -> creator = malloc(len*sizeof(char) + 1);
            strcpy(doc -> creator, cont);
        }
    }
}

void setWaypointProperties(xmlNode *cur_node, List *insertList){

    Waypoint *wpt = malloc(sizeof(Waypoint));
    wpt -> name = "";
    wpt -> latitude = 0;
    wpt -> longitude = 0;
    wpt -> otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    xmlAttr *attr;
    for (attr = cur_node->properties; attr != NULL; attr = attr->next){

        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);

        if(strcmp(attrName, "lat") == 0){
            char *ptr;
            wpt -> latitude = strtod(cont, &ptr);
        }
        if(strcmp(attrName, "lon") == 0){
            char *ptr;
            wpt -> longitude = strtod(cont, &ptr);
        }
    }

    xmlNode *the_node = NULL;
    for (the_node = cur_node->children; the_node != NULL; the_node = the_node->next){

        if(the_node->type == XML_ELEMENT_NODE && strcmp((char*)the_node->name, "name") == 0){
            char *content = (char*)xmlNodeGetContent(the_node);
            int lenName = strlen(content);
            wpt -> name = malloc(lenName*sizeof(char) + 1);
            strcpy(wpt -> name, content);
            xmlFree((xmlChar*)content);
        }
        else if(the_node->type == XML_ELEMENT_NODE && strcmp((char*)the_node->name, "name") != 0){
            setGPXDataProperties(the_node, wpt -> otherData);
        }
    }
    insertBack(insertList, (void*)wpt);
}

void setGPXDataProperties(xmlNode *cur_node, List *insertList){

    char *content = (char*)xmlNodeGetContent(cur_node);
    int lenContent = strlen(content) + 1;

    GPXData *new_data = malloc(sizeof(GPXData) + lenContent*sizeof(char));

    //copy the GPXData properties to the corresponding struct members 
    strcpy(new_data -> name, (char*)cur_node -> name);
    strcpy(new_data -> value, content);

    //frees malloc'd xmlChar* pointer created when using the xmlNodeGetContent function
    xmlFree((xmlChar*)content);

    //adds new GPXData structure to the front of the list it should be added to
    insertBack(insertList, new_data);
}

void setRouteProperties(xmlNode *cur_node, GPXdoc *doc){

    Route *rte = malloc(sizeof(Route));
    rte -> name = "";
    rte -> waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    rte -> otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    xmlNode *the_node = NULL;
    for (the_node = cur_node->children; the_node != NULL; the_node = the_node->next){

        if(the_node->type == XML_ELEMENT_NODE && strcmp((char*)the_node->name, "name") == 0){
            char *content = (char*)xmlNodeGetContent(the_node);
            int lenName = strlen(content);
            rte -> name = malloc(lenName*sizeof(char) + 1);
            strcpy(rte -> name, content);
            xmlFree((xmlChar*)content);
        }
        else if(the_node->type == XML_ELEMENT_NODE && strcmp((char*)the_node->name, "rtept") == 0){
            setWaypointProperties(the_node, rte -> waypoints);
        }
        else if(the_node->type == XML_ELEMENT_NODE && (strcmp((char*)the_node->name, "rtept") != 0 && strcmp((char*)the_node->name, "name") != 0)){
            setGPXDataProperties(the_node, rte -> otherData);
        }
    }
    insertBack(doc -> routes, (void*)rte);
}

void setTrackProperties(xmlNode *cur_node, GPXdoc *doc){

    Track *trk = malloc(sizeof(Track));
    trk -> name = "";
    trk -> segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
    trk -> otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    xmlNode *the_node = NULL;
    for (the_node = cur_node->children; the_node != NULL; the_node = the_node->next){

        if(the_node->type == XML_ELEMENT_NODE && strcmp((char*)the_node->name, "name") == 0){
            char *content = (char*)xmlNodeGetContent(the_node);
            int lenName = strlen(content);
            trk -> name = malloc(lenName*sizeof(char) + 1);
            strcpy(trk -> name, content);
            xmlFree((xmlChar*)content);
        }
        else if(the_node->type == XML_ELEMENT_NODE && strcmp((char*)the_node->name, "trkseg") == 0){
            setTrackSegmentProperties(the_node, trk -> segments);
        }
        else if(the_node->type == XML_ELEMENT_NODE && (strcmp((char*)the_node->name, "trkseg") != 0 && strcmp((char*)the_node->name, "name") != 0)){
            setGPXDataProperties(the_node, trk -> otherData);
        }
    }
    insertBack(doc -> tracks, (void*)trk);
}

void setTrackSegmentProperties(xmlNode *cur_node, List *insertList){

    TrackSegment *seg = malloc(sizeof(TrackSegment));
    seg -> waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    
    xmlNode *the_node = NULL;
    for (the_node = cur_node->children; the_node != NULL; the_node = the_node->next){
        if(the_node->type == XML_ELEMENT_NODE && strcmp((char*)the_node->name, "trkpt") == 0){
            setWaypointProperties(the_node, seg -> waypoints);
        }
    }
    insertBack(insertList, (void*)seg);
}

char *concatFunc(char *oldString, char *newString){

    const size_t len1 = strlen(oldString);
    const size_t len2 = strlen(newString);
    const size_t newSize = len1 + len2 + 1;

    oldString = realloc(oldString, newSize);
    memcpy(oldString + len1, newString, len2 + 1);

    return oldString;
}

/**************************************** A2 Functions *******************************************/

/**
 * This function converts a GPXdoc into an xmlDoc struct for validation against XSD file
 * Sources Used: 
 * http://www.xmlsoft.org/examples/tree2.c
 * libXmlExample.c file from course materials
 */
xmlDocPtr convertGPXtoXml(GPXdoc *gpx){

    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;
    //xmlNodePtr node1 = NULL;
    //xmlNodePtr node2 = NULL;

    LIBXML_TEST_VERSION;

    // Creates a new document, a node and set it as a root node
    doc = xmlNewDoc((xmlChar*)"1.0");
    root_node = xmlNewNode(NULL, (xmlChar*)"gpx");

    // Sets the root element of the document
    xmlDocSetRootElement(doc, root_node);

    // Sets the namespace of the document
    xmlNsPtr namespace = xmlNewNs(root_node, (xmlChar*) gpx -> namespace , NULL);
    xmlSetNs(root_node, namespace);

    // Sets the version and creator from the GPXdoc
    char ptr[10];
    sprintf(ptr, "%.1f", gpx -> version);
    xmlNewProp(root_node, (xmlChar*)"version", (xmlChar*)ptr);
    xmlNewProp(root_node, (xmlChar*)"creator", (xmlChar*)gpx -> creator);

    // Sets the waypoints of from the GPXdoc
    if(getLength(gpx -> waypoints) != 0){

        ListIterator iter = createIterator(gpx -> waypoints);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            Waypoint *wpt = (Waypoint*)elem;
            xmlNodePtr waypointNode = xmlNewChild(root_node, NULL, (xmlChar*)"wpt", NULL);
            waypointToXml(wpt, waypointNode);
        }
    }

    // Sets the routes of from the GPXdoc
    if(getLength(gpx -> routes) != 0){

        ListIterator iter = createIterator(gpx -> routes);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            Route *rte = (Route*)elem;
            xmlNodePtr routeNode = xmlNewChild(root_node, NULL, (xmlChar*)"rte", NULL);
            routeToXml(rte, routeNode);
        }
    }

    // Sets the tracks of from the GPXdoc
    if(getLength(gpx -> tracks) != 0){

        ListIterator iter = createIterator(gpx -> tracks);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            Track *trk = (Track*)elem;
            xmlNodePtr trackNode = xmlNewChild(root_node, NULL, (xmlChar*)"trk", NULL);
            trackToXml(trk, trackNode);
        }
    } 

    return doc;
}

/**
 * Converts a Waypoint struct to an xmlNode
 **/
void waypointToXml(Waypoint *wpt, xmlNodePtr node){

    // Sets the name of the waypoint if it is not an empty string
    if(strcmp(wpt -> name, "") != 0){
        xmlNewChild(node, NULL, (xmlChar*)"name", (xmlChar*) wpt -> name);
    }

    // Sets the latitude of the waypoint
    char lat[10];
    sprintf(lat, "%.5f", wpt -> latitude);
    xmlNewProp(node, (xmlChar*)"lat", (xmlChar*)lat);

    // Sets the longitude of the waypoint
    char lon[10];
    sprintf(lon, "%.5f", wpt -> longitude);
    xmlNewProp(node, (xmlChar*)"lon", (xmlChar*)lon);

    // Sets the otherData of the waypoint if the list is not empty
    if(getLength(wpt -> otherData) != 0){

        ListIterator iter = createIterator(wpt -> otherData);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            GPXData *data = (GPXData*)elem;
            xmlNewChild(node, NULL, (xmlChar*)data -> name, (xmlChar*)data -> value);
        }
    }
}

/**
 * Converts a Route struct to an xmlNode
 **/
void routeToXml(Route *rte, xmlNodePtr node){

    // Sets the name of the route if it is not an empty string
    if(strcmp(rte -> name, "") != 0){
        xmlNewChild(node, NULL, (xmlChar*)"name", (xmlChar*) rte -> name);
    }

    // Sets the otherData of the route if the list is not empty
    if(getLength(rte -> otherData) != 0){

        ListIterator iter = createIterator(rte -> otherData);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            GPXData *data = (GPXData*)elem;
            xmlNewChild(node, NULL, (xmlChar*)data -> name, (xmlChar*)data -> value);
        }
    }

    // Sets the waypoints of the route if the list is not empty
    if(getLength(rte -> waypoints) != 0){

        ListIterator iter = createIterator(rte -> waypoints);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            Waypoint *wpt = (Waypoint*)elem;
            xmlNodePtr waypointNode = xmlNewChild(node, NULL, (xmlChar*)"rtept", NULL);
            waypointToXml(wpt, waypointNode);
        }
    }
}

/**
 * Converts a Track struct to an xmlNode
 **/
void trackToXml(Track *trk, xmlNodePtr node){

    // Sets the name of the route if it is not an empty string
    if(strcmp(trk -> name, "") != 0){
        xmlNewChild(node, NULL, (xmlChar*)"name", (xmlChar*) trk -> name);
    }

    // Sets the otherData of the route if the list is not empty
    if(getLength(trk -> otherData) != 0){

        ListIterator iter = createIterator(trk -> otherData);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            GPXData *data = (GPXData*)elem;
            xmlNewChild(node, NULL, (xmlChar*)data -> name, (xmlChar*)data -> value);
        }
    }

    // Sets the segments of the route if the list is not empty
    if(getLength(trk -> segments) != 0){

        ListIterator iter = createIterator(trk -> segments);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            TrackSegment *seg = (TrackSegment*)elem;
            xmlNodePtr child = xmlNewChild(node, NULL, (xmlChar*)"trkseg", NULL);

            // Sets the waypoints of the segment if the list is not empty
            if(getLength(seg -> waypoints) != 0){

                ListIterator iter2 = createIterator(seg -> waypoints);
                void *elem2;

                while((elem2 = nextElement(&iter2)) != NULL){
                    Waypoint *wpt = (Waypoint*)elem2;
                    xmlNodePtr waypointNode = xmlNewChild(child, NULL, (xmlChar*)"trkpt", NULL);
                    waypointToXml(wpt, waypointNode);
                }
            }
        }
    }
}

/**
 * This function validates a libxml tree
 * Source Used: http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
 **/

bool validateXMLTree(){

    bool isValid = false;

    return isValid;
}


/** The following set of functions validate the constraints from GPXParser.h for validateGPXDoc **/

/**
 * Checks whether GPXdoc struct violates any constraints specified in GPXParser.h
 * All pointers in a GPXdoc must be initialized and must not be NULL
 * Checks that numbers are within valid ranges, lists are not NULL, ect.
 **/
bool validatesConstraints(GPXdoc *doc){

    bool isValid = true;

    // Checks that namespace is not an empty string
    if(doc -> namespace == NULL || strcmp(doc -> namespace, "") == 0){
        return false;
    }

    // Checks that version is initialized

    // Checks that creator is not an empty string
    if(doc -> creator == NULL || strcmp(doc -> creator, "") == 0){
        return false;
    }

    // Checks that waypoints is not NULL, can be empty
    // Othwerwise check each waypoint's validity
    if(doc -> waypoints == NULL){
        return false;
    }
    else{
        ListIterator iter = createIterator(doc -> waypoints);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            Waypoint *wpt = (Waypoint*)elem;

            if(validWaypoint(wpt) == false){
                return false;
            }
        }
    }

    // Checks that routes is not NULL, can be empty
    // Othwerwise check each route's validity
    if(doc -> routes == NULL){
        return false;
    }
    else{
    
        ListIterator iter = createIterator(doc -> routes);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            Route *rte = (Route*)elem;

            if(validRoute(rte) == false){
                return false;
            }
        }
    }

    // Checks that tracks is not NULL, can be empty
    // Othwerwise check each track's validity
    if(doc -> tracks == NULL){
        return false;
    }
    else{

        ListIterator iter = createIterator(doc -> tracks);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            Track *trk = (Track*)elem;

            if(validTrack(trk) == false){
                return false;
            }
        }
    }
    return isValid;
}

/**
 * Validates a GPXData struct
 * Returns true if valid struct, false otherwise
 **/
bool validData(GPXData *data){

    bool isValid = true;

    // Checks that name is not an empty string
    if(data -> name == NULL || strcmp(data -> name, "") == 0){
        return false;
    }

    // Checks that value is not an empty string
    else if(data -> value == NULL || strcmp(data -> value, "") == 0){
        return false;
    }

    return isValid;
}

/**
 * Validates a Waypoint struct
 * Returns true if valid struct, false otherwise
 **/
bool validWaypoint(Waypoint *wpt){

    bool isValid = true;

    // Checks that name is not NULL, can be empty string
    if(wpt -> name == NULL){
        return false;
    }

    // Checks that latitude and longitude have been initialized

    // Checks that otherData is not NULL, list may be empty
    // Otherwise checks that each GPXData in list is valid
    if(wpt -> otherData == NULL){
        return false;
    }
    else{
        ListIterator iter = createIterator(wpt -> otherData);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            GPXData *data = (GPXData*)elem;

            if(validData(data) == false){
                return false;
            }
        }
    }

    return isValid;
}

/**
 * Validates a Route struct
 * Returns true if valid struct, false otherwise
 **/
bool validRoute(Route *rte){

    bool isValid = true;

    // Checks that name is not NULL, can be empty string
    if(rte -> name == NULL){
        return false;
    }

    // Checks that waypoints is not NULL, list may be empty
    // Otherwise checks that each Waypoint in list is valid
    if(rte -> waypoints == NULL){
        return false;
    }
    else{
        ListIterator iter = createIterator(rte -> waypoints);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            Waypoint *wpt = (Waypoint*)elem;

            if(validWaypoint(wpt) == false){
                return false;
            }
        }
    }

    // Checks that otherData is not NULL, list may be empty
    // Otherwise checks that each GPXData in list is valid
    if(rte -> otherData == NULL){
        return false;
    }
    else{
        ListIterator iter = createIterator(rte -> otherData);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            GPXData *data = (GPXData*)elem;

            if(validData(data) == false){
                return false;
            }
        }
    }

    return isValid;
}

/**
 * Validates a TrackSegment struct
 * Returns true if valid struct, false otherwise
 **/
bool validTrackSegment(TrackSegment *seg){

    bool isValid = true;

    // Checks that waypoints is not NULL, list may be empty
    // Otherwise checks that each Waypoint in list is valid
    if(seg -> waypoints == NULL){
        return false;
    }
    else{
        ListIterator iter = createIterator(seg -> waypoints);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            Waypoint *wpt = (Waypoint*)elem;

            if(validWaypoint(wpt) == false){
                return false;
            }
        }
    }
    return isValid;
}

/**
 * Validates a Track struct
 * Returns true if valid struct, false otherwise
 **/
bool validTrack(Track *trk){

    bool isValid = true;

    // Checks that name is not NULL, can be empty string
    if(trk -> name == NULL){
        return false;
    }

    // Checks that segments is not NULL, list may be empty
    // Otherwise checks that each segment in list is valid
    if(trk -> segments == NULL){
        return false;
    }
    else{
        ListIterator iter = createIterator(trk -> segments);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            TrackSegment *seg = (TrackSegment*)elem;

            if(validTrackSegment(seg) == false){
                return false;
            }
        }
    }

    // Checks that otherData is not NULL, list may be empty
    // Otherwise checks that each GPXData in list is valid
    if(trk -> otherData == NULL){
        return false;
    }
    else{
        ListIterator iter = createIterator(trk -> otherData);
        void *elem;

        while((elem = nextElement(&iter)) != NULL){
            GPXData *data = (GPXData*)elem;

            if(validData(data) == false){
                return false;
            }
        }
    }

    return isValid;
}


/**
 * Uses the haversine formula to compute the distance between two waypoints
 **/
float computeHaversine(double lat1, double lat2, double lon1, double lon2){

    float R = 6371e3;
    float phi1 = lat1 * (M_PI/180);
    float phi2 = lat2 * (M_PI/180);
    float deltaPhi = (lat2 - lat1) * (M_PI/180);
    float deltaLambda = (lon2 - lon1) * (M_PI/180);

    float a = sin(deltaPhi/2)*sin(deltaPhi/2) + cos(phi1)*cos(phi2)*sin(deltaLambda/2)*sin(deltaLambda/2);

    float c = 2 * atan2(sqrt(a), sqrt(1-a));

    float d = R * c;

    return d;
}

void deleteRouteDummy(void* data){

}

void deleteTrackDummy(void* data){
    
}

/************************************************A3 Helper Functions*****************************************************/

char* addToFileLog(char* fileName){

    // Appends the upload directory to get to the file
    char *fileDirectory = "";
    fileDirectory = malloc(sizeof(char) * 12);
    strcpy(fileDirectory, "uploads/");
    fileDirectory = concatFunc(fileDirectory, fileName);

    // Validates and creates a new GPXdoc struct
    GPXdoc *doc = createGPXdoc(fileDirectory);
    free(fileDirectory);

    bool isValid = validateGPXDoc(doc, "parser/gpx.xsd");

    if(!isValid){
        deleteGPXdoc(doc);
        return "Invalid";
    }

    char *json = GPXtoJSON(doc);
    deleteGPXdoc(doc);

    return json;
}

char* addToRouteDisplay(char* fileName){

    // Appends the upload directory to get to the file
    char *fileDirectory = "";
    fileDirectory = malloc(sizeof(char) * 12);
    strcpy(fileDirectory, "uploads/");
    fileDirectory = concatFunc(fileDirectory, fileName);

    // Validates and creates a new GPXdoc struct
    GPXdoc *doc = createGPXdoc(fileDirectory);
    free(fileDirectory);

    bool isValid = validateGPXDoc(doc, "parser/gpx.xsd");

    if(!isValid){
        deleteGPXdoc(doc);
        return "Invalid";
    }

    char *json = routeListToJSON(doc -> routes);
    deleteGPXdoc(doc);

    return json;

}

char* addToTrackDisplay(char* fileName){

    // Appends the upload directory to get to the file
    char *fileDirectory = "";
    fileDirectory = malloc(sizeof(char) * 12);
    strcpy(fileDirectory, "uploads/");
    fileDirectory = concatFunc(fileDirectory, fileName);

    // Validates and creates a new GPXdoc struct
    GPXdoc *doc = createGPXdoc(fileDirectory);
    free(fileDirectory);

    bool isValid = validateGPXDoc(doc, "parser/gpx.xsd");

    if(!isValid){
        deleteGPXdoc(doc);
        return "Invalid";
    }

    char *json = trackListToJSON(doc -> tracks);
    deleteGPXdoc(doc);

    return json; 
}

char *createGPXFromInput(char *fileName, float version, char *creator){

    // Appends the upload directory to get to the file
    char *fileDirectory = "";
    fileDirectory = malloc(sizeof(char) * 12);
    strcpy(fileDirectory, "uploads/");
    fileDirectory = concatFunc(fileDirectory, fileName);

    // {"version":ver,"creator":"creatorValue"}
    char *jsonStr = "";
    jsonStr = malloc(sizeof(char) * 12);
    
    // Sets version
    strcpy(jsonStr, "{\"version\":");
    char versiontoString[20];
    sprintf(versiontoString, "%.1f", version);
    jsonStr = concatFunc(jsonStr, versiontoString);

    // Sets creator
    jsonStr = concatFunc(jsonStr, ",\"creator\":\"");
    jsonStr = concatFunc(jsonStr, creator);

    GPXdoc *doc = JSONtoGPX(jsonStr);

    bool writeStatus = writeGPXdoc(doc, fileDirectory);
    free(fileDirectory);

    if(writeStatus == false){
        return "Invalid";
    }

    char *json = GPXtoJSON(doc);
    deleteGPXdoc(doc);

    return json;
}

char *getOtherDataRoute(char *fileName, int index){

    // Appends the upload directory to get to the file
    char *fileDirectory = "";
    fileDirectory = malloc(sizeof(char) * 12);
    strcpy(fileDirectory, "uploads/");
    fileDirectory = concatFunc(fileDirectory, fileName);

    // Validates and creates a new GPXdoc struct
    GPXdoc *doc = createGPXdoc(fileDirectory);
    free(fileDirectory);

    // Defines variables to iterate over list
    ListIterator iter = createIterator(doc -> routes);
    void *elem;
    int i = 0;
    Route *selectedRoute;

    // Iterates until the route with matching index is found and overwrites its name
    while((elem = nextElement(&iter)) != NULL){

        Route *rte = (Route*)elem;
        if(i == index){
            selectedRoute = rte;
            break;
        }
        i++;
    }

    char *json = dataListToJSON(selectedRoute -> otherData);
    deleteGPXdoc(doc);

    return json;
}

char *getOtherDataTrack(char *fileName, int index){

    // Appends the upload directory to get to the file
    char *fileDirectory = "";
    fileDirectory = malloc(sizeof(char) * 12);
    strcpy(fileDirectory, "uploads/");
    fileDirectory = concatFunc(fileDirectory, fileName);

    // Validates and creates a new GPXdoc struct
    GPXdoc *doc = createGPXdoc(fileDirectory);
    free(fileDirectory);

    // Defines variables to iterate over list
    ListIterator iter = createIterator(doc -> tracks);
    void *elem;
    int i = 0;
    Track *selectedTrack;

    // Iterates until the route with matching index is found and overwrites its name
    while((elem = nextElement(&iter)) != NULL){

        Track *trk = (Track*)elem;
        if(i == index){
            selectedTrack = trk;
            break;
        }
        i++;
    }

    char *json = dataListToJSON(selectedTrack -> otherData);
    deleteGPXdoc(doc);

    return json;
}

void renameSelectedRoute(char *fileName, char *name, int index){

    // Appends the upload directory to get to the file
    char *fileDirectory = "";
    fileDirectory = malloc(sizeof(char) * 12);
    strcpy(fileDirectory, "uploads/");
    fileDirectory = concatFunc(fileDirectory, fileName);

    // Creates a GPXdoc struct
    GPXdoc *doc = createGPXdoc(fileDirectory);

    // Defines variables to iterate over list
    ListIterator iter = createIterator(doc -> routes);
    void *elem;
    int i = 0;

    // Iterates until the route with matching index is found and overwrites its name
    while((elem = nextElement(&iter)) != NULL){

        Route *rte = (Route*)elem;
        if(i == index){
            strcpy(rte -> name, name);
            break;
        }
        i++;
    }

    bool writeStatus = writeGPXdoc(doc, fileDirectory);
    free(fileDirectory);

    if(writeStatus == false){
        return;
    }

    deleteGPXdoc(doc);
}

void renameSelectedTrack(char *fileName, char *name, int index){

    // Appends the upload directory to get to the file
    char *fileDirectory = "";
    fileDirectory = malloc(sizeof(char) * 12);
    strcpy(fileDirectory, "uploads/");
    fileDirectory = concatFunc(fileDirectory, fileName);

    // Creates a GPXdoc struct
    GPXdoc *doc = createGPXdoc(fileDirectory);

    // Defines variables to iterate over list
    ListIterator iter = createIterator(doc -> tracks);
    void *elem;
    int i = 0;

    // Iterates until the track with matching index is found and overwrites its name
    while((elem = nextElement(&iter)) != NULL){

        Track *trk = (Track*)elem;
        if(i == index){
            strcpy(trk -> name, name);
            break;
        }
        i++;
    }

    bool writeStatus = writeGPXdoc(doc, fileDirectory);
    free(fileDirectory);

    if(writeStatus == false){
        return;
    }

    deleteGPXdoc(doc);
}

void addRouteToFile(char *fileName){

}

char *displayRoutesBetween(char *fileName, float latitudeStart, float longitudeStart, float latitudeEnd, float longitudeEnd, float comp){

    // Appends the upload directory to get to the file
    char *fileDirectory = "";
    fileDirectory = malloc(sizeof(char) * 12);
    strcpy(fileDirectory, "uploads/");
    fileDirectory = concatFunc(fileDirectory, fileName);

    // Validates and creates a new GPXdoc struct
    GPXdoc *doc = createGPXdoc(fileDirectory);
    free(fileDirectory);

    // Gets the list of routes based on input coordinates
    List *rteBetween = getRoutesBetween(doc, latitudeStart, longitudeStart, latitudeEnd, longitudeEnd, comp);
    
    char *json = routeListToJSON(rteBetween);

    deleteGPXdoc(doc);

    return json;
}

char *displayTracksBetween(char *fileName, float latitudeStart, float longitudeStart, float latitudeEnd, float longitudeEnd, float comp){
    
    // Appends the upload directory to get to the file
    char *fileDirectory = "";
    fileDirectory = malloc(sizeof(char) * 12);
    strcpy(fileDirectory, "uploads/");
    fileDirectory = concatFunc(fileDirectory, fileName);

    // Validates and creates a new GPXdoc struct
    GPXdoc *doc = createGPXdoc(fileDirectory);
    free(fileDirectory);

    // Gets the list of tracks based on input coordinates
    List *trkBetween = getTracksBetween(doc, latitudeStart, longitudeStart, latitudeEnd, longitudeEnd, comp);
    
    char *json = trackListToJSON(trkBetween);

    deleteGPXdoc(doc);

    return json;
}