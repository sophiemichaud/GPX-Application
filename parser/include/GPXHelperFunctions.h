/*
    * Name: Sophie Michaud
    * Student ID: 0999546
*/

#ifndef GPX_HELPER_FUNCTIONS_H
#define GPX_HELPER_FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>
#include "GPXParser.h"
#include "LinkedListAPI.h"

/*********************** Assignment 1 - Module 1 functions *************************/

GPXdoc *createGPXdocHelper(xmlNode *root, GPXdoc *doc);

void setGPXDocProperties(xmlNode *cur_node, GPXdoc *doc);

void setWaypointProperties(xmlNode *cur_node, List *insertList);

void setGPXDataProperties(xmlNode *cur_node, List *insertList);

void setRouteProperties(xmlNode *cur_node, GPXdoc *doc);

void setTrackProperties(xmlNode *cur_node, GPXdoc *doc);

void setTrackSegmentProperties(xmlNode *cur_node, List *insertList);

char *concatFunc(char *oldString, char *newString);

/*********************** Assignment 2 - Module 1 functions *************************/

xmlDocPtr convertGPXtoXml(GPXdoc *gpx);

void waypointToXml(Waypoint *wpt, xmlNodePtr node);

void routeToXml(Route *rte, xmlNodePtr node);

void trackToXml(Track *trk, xmlNodePtr node);

bool validateXMLTree();

bool validatesConstraints(GPXdoc *doc);

bool validData(GPXData *data);

bool validWaypoint(Waypoint *wpt);

bool validRoute(Route *rte);

bool validTrackSegment(TrackSegment *seg);

bool validTrack(Track *trk);

float computeHaversine(double lat1, double lat2, double lon1, double lon2);

void deleteRouteDummy(void* data);

void deleteTrackDummy(void* data);

char* addToFileLog(char* fileName);

char* addToRouteDisplay(char* fileName);

char* addToTrackDisplay(char* fileName);

void renameSelectedRoute(char *fileName, char *name, int index);

void renameSelectedTrack(char *fileName, char *name, int index);

char *getOtherDataTrack(char *fileName, int index);

char *getOtherDataRoute(char *fileName, int index);

char *createGPXFromInput(char *fileName, float version, char *creator);

char *displayRoutesBetween(char *fileName, float latitudeStart, float longitudeStart, float latitudeEnd, float longitudeEnd, float comp);

char *displayTracksBetween(char *fileName, float latitudeStart, float longitudeStart, float latitudeEnd, float longitudeEnd, float comp);

#endif