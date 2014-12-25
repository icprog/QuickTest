#ifndef CLIENTXMLIMPL_H
#define CLIENTXMLIMPL_H

#include "clientdatadefine.h"

#include <QtXml/QDomDocument>
#include <QtXml/QXmlDefaultHandler>


class DSE_XmlSaxHandler : public QXmlDefaultHandler
{
public:
	DSE_XmlSaxHandler(QObject *parent= NULL);
	~DSE_XmlSaxHandler();

	QDomElement createNode(QDomDocument &doc,QDomElement &parentElement,QString nodeName,QStringList &list );



	virtual bool analystStartEmelent(const QString &namespaceURI, const QString &localName,
                       const QString &qName, const QXmlAttributes &attributes){Q_UNUSED(namespaceURI); Q_UNUSED(localName); Q_UNUSED(qName); Q_UNUSED(attributes); return true;};
	virtual bool analystEndEmelent(const QString &namespaceURI, const QString &localName,
        const QString &qName){Q_UNUSED(namespaceURI); Q_UNUSED(localName); Q_UNUSED(qName); return true;};

    virtual bool analystCharacters(const QString &str){ Q_UNUSED(str); return true;};

	int readFile(const char *fileName,char* err = NULL);
	int writeFile(QDomDocument &doc,const char *fileName,char* err = NULL,QString codec = "GBK");
	
	void createAttrib(QDomDocument &doc,QDomElement &element,QStringList &list );

	qint64 getFileSize(){return m_fileSize;};

private:


	 bool startElement(const QString &namespaceURI, const QString &localName,
                       const QString &qName, const QXmlAttributes &attributes);
     bool endElement(const QString &namespaceURI, const QString &localName,
                     const QString &qName);
     bool characters(const QString &str);
     bool fatalError(const QXmlParseException &exception);
     QString errorString() const;

	  qint64  m_fileSize ;
	
};




//系统参数实现类
class  DSE_SysImpl : public DSE_XmlSaxHandler
{
public:

	DSE_SysImpl(){};
	~DSE_SysImpl(){clear();};

	int read(char *fileName,  char* err = NULL);

	int write(char *fileName,char* err = NULL);

	SYSNODE  m_sysNode;          //系统节点
	ENVIRNODE  m_envirNode;      //环境节点
	USERNODE   m_userNode;       //用户节点

private:

	
	bool     m_bDomain;
	bool     m_bSta;
	bool     m_bEqu;

	PSTANODE m_pStaNode;
	PDOMAINNODE m_pDomainNode;

	void	initVars();

	virtual bool analystStartEmelent(const QString &namespaceURI, const QString &localName,
                       const QString &qName, const QXmlAttributes &attributes);
	virtual bool analystEndEmelent(const QString &namespaceURI, const QString &localName,
						const QString &qName);
	virtual bool analystCharacters(const QString &str);

    void createXMlNode(QDomDocument &doc);

	void clear();

};







#endif // XMLIMPL_H
