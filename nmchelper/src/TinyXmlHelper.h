#ifndef TINYXML_HELPER_H
#define TINYXML_HELPER_H


#if (defined(WIN32) || defined(WIN64))
#include <tinyxml2/tinyxml2.h>
//#include "tinyxml2.h"
#else
#pragma GCC visibility push(hidden)
#include <tinyxml2/tinyxml2.h>
//#include "tinyxml2.h"
#pragma GCC visibility pop
#endif

#include <stdio.h>
using namespace tinyxml2;

#ifndef XML_DOC_INIT
#define XML_DOC_INIT(xDoc) xDoc.Parse( "<?xml version=\"1.0\" ?>\n" )
#endif

#ifndef XML_DOC_INIT_EX
#define XML_DOC_INIT_EX(xDoc,XML) xDoc.Parse((XML))
#endif

#define SAFE_ITEM(X) ((X))?(X):""        //默认为空
#define SAFE_ITEM_EX(X,Y) ((X))?(X):(Y)  //默认为"Y"

#define DECLARE_NODE(nodename,topnode)  \
	XMLElement* nodename##Node = topnode->FirstChildElement(#nodename); \
	do{ \
		if(nodename##Node==NULL) \
		{	\
			printf("[%s]%s, %s is null\n", __FUNCTION__, #topnode, #nodename); \
			return -2; \
		} \
	} \
	while(0)

#define ADD_ATTR(node,attrname,attr) node->SetAttribute(#attrname,attr)
#define GET_ATTR(attrname,node) const char* ch##attrname = SAFE_ITEM(node->Attribute(#attrname))
#define GET_TEXT(node) const char* ch##node = SAFE_ITEM(node##Node->GetText())

#define ADD_NODE(doc,newnode,topnode) \
			XMLElement* newnode##Node = doc.NewElement(#newnode); \
			do{ \
				if(newnode##Node==NULL) \
				{	\
					printf("[%s]%s, %s is null\n", __FUNCTION__, #topnode, #newnode); \
					return -2; \
				} \
				topnode->LinkEndChild(newnode##Node); \
			} \
			while(0)


#define ADD_NODE_WITH_TEXT(doc,newnode,topnode,text) \
	XMLElement* newnode##Node = doc.NewElement(#newnode); \
	do{ \
		if(newnode##Node==NULL) \
		{	\
			printf("[%s]%s, %s is null\n", __FUNCTION__, #topnode, #newnode); \
			return -2; \
		} \
		newnode##Node->LinkEndChild(doc.NewText(text)); \
		topnode->LinkEndChild(newnode##Node); \
	} \
	while(0)









































#endif
