#include "XMLWrap.h"


CXMLWrap::CXMLWrap(void)
{

}
CXMLWrap::~CXMLWrap(void)
{

}
bool CXMLWrap::ParseXmlString(const char* s)
{
	 m_xml.Parse(s);
	 if (m_xml.Error())
	 {
		 int nError = m_xml.ErrorId();
		 printf("%s",m_xml.ErrorDesc());
		 return false;
	 }
	 return true;
}
bool CXMLWrap::ParseXmlFile(const char* xmlFile) 
{ 
	m_xml.LoadFile(xmlFile);
	if (m_xml.Error())
	{
		int nError = m_xml.ErrorId();
		printf("%s",m_xml.ErrorDesc());
		return false;
	}
	return true;
} 
TiXmlElement* CXMLWrap::GetRootElement() 
{ 
	return m_xml.RootElement(); 
} 
TiXmlElement * CXMLWrap::GetChildElement(TiXmlElement * pParentElement,const char * title)
{
	if (pParentElement != NULL)
	{
		TiXmlNode * childNode = GetFirstChildElement(pParentElement);
		for (;childNode;childNode = GetNextChildElement(childNode->ToElement()))
		{
			if (!strcmp(title,childNode->Value()))
			{
				return childNode->ToElement();
			}
		}
	}
	return NULL;
}
TiXmlElement * CXMLWrap::GetFirstChildElement(TiXmlElement * pParentElement)
{
	if (pParentElement != NULL)
	{
		TiXmlElement * child = pParentElement->FirstChildElement();
		return child;
	}
	return NULL;
}
TiXmlElement * CXMLWrap::GetNextChildElement(TiXmlElement * pElement)
{
	if (pElement)
	{
		TiXmlElement * nextChild = pElement->NextSiblingElement();
		return nextChild;
	}
	return NULL;
}
std::string CXMLWrap::GetElementValue(TiXmlElement * Element)
{
	std::string reslut = "\0";
	if (Element != NULL)
	{
		const char* pChar = Element->GetText();
		if (pChar == NULL)
		{
			return "";
		}

		reslut = pChar;
	}
	return reslut;
}
std::string CXMLWrap::GetElementAttributeValue(TiXmlElement* Element,const char*AttributeName) 
{ 
	std::string reslut = "\0";
	if (Element != NULL)
	{
		reslut = Element->Attribute(AttributeName); 
	}
	return reslut; 
}
std::string CXMLWrap::GetChildElementValue(TiXmlElement * pParentElement,const char * title)
{
	std::string reslut = "\0";
	TiXmlElement * pElem = GetChildElement(pParentElement,title);
	reslut = GetElementValue(pElem);
	return reslut;
}
std::string CXMLWrap::GetChildElementAttributeValue(TiXmlElement * pParentElement,const char *title,const char* AttributeName)
{
	std::string reslut = "\0";
	TiXmlElement * pElem = GetChildElement(pParentElement,title);
	reslut = GetElementAttributeValue(pElem,AttributeName);
	return reslut;
}
void CXMLWrap::Clear() 
{ 
	m_xml.Clear(); 
} 
//////////////////////////////////////////////////////////////////////////
TiXmlElement* CXMLWrap::AddXmlRootElement(const char* title) 
{ 
	TiXmlElement* element =new TiXmlElement(title); 
	m_xml.LinkEndChild(element); 
	return element; 
} 
TiXmlElement* CXMLWrap::AddXmlChildElement(TiXmlElement* pPareElement,const char* title,const char * value) 
{ 
	if(pPareElement) 
	{ 
		TiXmlElement* childElem =new TiXmlElement(title); 
		pPareElement->LinkEndChild(childElem); 
		if (value != "\0")
		{
			AddElementValue(childElem,value);
		}
		return childElem; 
	} 
	return 0; 
} 
void CXMLWrap::AddXmlAttribute(TiXmlElement* pElement,const char* name,const char* value) 
{ 
	if(pElement) 
	{ 
		pElement->SetAttribute(name,value); 
	} 
} 
void CXMLWrap::AddXmlDeclaration(const char* vesion,const char* encoding,const char*standalone) 
{ 
	TiXmlDeclaration * decl =new TiXmlDeclaration(vesion,encoding,standalone); 
	m_xml.LinkEndChild(decl); 
} 

void CXMLWrap::AddElementValue(TiXmlElement *pElement,const char* value) 
{ 
	if(pElement) 
	{ 
		TiXmlText * text =new TiXmlText(value); 
		pElement->LinkEndChild(text); 
	} 
} 

void CXMLWrap::AddXmlComment(TiXmlElement* pElement,const char* Comment) 
{ 
	if(pElement) 
	{ 
		TiXmlComment * comment =new TiXmlComment(Comment); 
		pElement->LinkEndChild(comment); 
	} 
} 

bool CXMLWrap::ReplaceElementValue(TiXmlElement* pElement,const char * newValue)
{
	if (pElement != NULL)
	{
		TiXmlNode * oldNode = NULL;
		oldNode = pElement->FirstChild();
		if (oldNode != NULL)
		{
			TiXmlText newText(newValue);
			pElement->ReplaceChild(oldNode,newText);
			return true;
		}
	}
	return false;
}

bool CXMLWrap::ReplaceElementAttribute(TiXmlElement* pElement,const char * name,const char *newValue)
{
	if (pElement != NULL)
	{
		TiXmlAttribute * oldAttribute = NULL;
		oldAttribute = pElement->FirstAttribute();
		for(;oldAttribute != NULL;oldAttribute = oldAttribute->Next())
		{
			if (!strcmp(name,oldAttribute->Name()))
			{
				oldAttribute->SetValue(newValue);
			}
		}
	}
	return false;
}

void CXMLWrap::SaveFile(const char* file) 
{ 
	m_xml.SaveFile(file); 
} 

TiXmlElement * CXMLWrap::FindFirstElement(const char * title)
{
	if(!title)
		return 0;
	TiXmlElement * pElem = NULL;
	pElem = GetRootElement();
	pElem = FindFirstElement(pElem,title);
	return pElem;
}

TiXmlElement * CXMLWrap::FindFirstElement(TiXmlElement* pcrElement,const char * title)
{
	TiXmlNode * pNode = pcrElement;
	while(pNode)
	{
		if(strcmp(pNode->Value(),title)==0)
		{
			return pNode->ToElement();
		} 
		else
		{
			TiXmlNode * nextElement= pNode->FirstChildElement();
			while(nextElement)
			{
				if(strcmp(nextElement->Value(),title)==0)
				{
					return nextElement->ToElement();
				}
				else
				{
					TiXmlElement* reElement = NULL;
					reElement = FindFirstElement(nextElement->ToElement(),title);
					if(reElement)
					{
						return reElement;
					}
				}
				nextElement = nextElement->NextSiblingElement();
			}
		}
		pNode = pNode->NextSiblingElement();
	}
	return NULL;
}
