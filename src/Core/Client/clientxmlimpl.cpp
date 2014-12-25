#include "clientxmlimpl.h"

#include <QtCore/QDir>
#include <QtDebug>
#include <QtGui/QApplication>
#include <QtCore/QTextCodec>

DSE_XmlSaxHandler::DSE_XmlSaxHandler(QObject *parent)
	
{

}

DSE_XmlSaxHandler::~DSE_XmlSaxHandler()
{

}



int DSE_XmlSaxHandler::readFile(const char *fileName,char* err )
 {

	 QXmlSimpleReader reader;
     reader.setContentHandler(this);
     reader.setErrorHandler(this);
	
	 QFile file(fileName);
     if (!file.open(QFile::ReadOnly | QFile::Text))
	 {   
		 t_char str[256];
		 sprintf(str,"file:%s open err",fileName);
		 printf("%s\n",str);
		 QString str11 = file.errorString();
		 QDir  dir;
		 QString strdir = dir.filePath(QString(fileName));
		
         return -1;
     }

	 m_fileSize = file.size();

	 //QXmlInputSource xmlInputSource;
	 QXmlInputSource xmlInputSource(&file);


	QString tmpFileName = QString("%1").arg(fileName);

	if (tmpFileName.contains(".xml"))
	{
		  QTextStream stream(&file);
		  QTextCodec *codec = QTextCodec::codecForName("GBK");
		  stream.setCodec(codec);

		  QString strData = stream.readAll();

		  xmlInputSource.setData(strData);
	}

     reader.parse(xmlInputSource);
	 file.close();


	 return 0;
 }

int DSE_XmlSaxHandler::writeFile(QDomDocument &doc,const char *fileName,char* err ,QString codec)
{

	char path[256];
	memset(path,0x00,sizeof(path));

	QString destFileName = QString("%1").arg(fileName);

	int  index = destFileName.lastIndexOf("/");
	QString strPath = destFileName.mid(0,index);

    QDir dir;
	dir.mkpath(strPath);

	QFile xmlFile(fileName);

	if(!xmlFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
	{	
		char str[256];
		sprintf(str,"file:%s create err",fileName);
		return -1 ;
	}

	QTextStream out(&xmlFile);
	//out.setCodec("GBK");
	out.setCodec(codec.toAscii().data());

	//doc.save(out, 4,QDomNode::EncodingFromTextStream);
	doc.save(out, 4);

	int v = -1;

	//马上把文件写进硬盘
	xmlFile.flush();
	#if !defined(Q_OS_WIN)
	 v = ::fsync(xmlFile.handle());
	#endif

	xmlFile.close();

	return 0 ;

}

void DSE_XmlSaxHandler::createAttrib(QDomDocument &doc,QDomElement &element,QStringList &list )
{

	for (int i = 0 ; i < list.count()  ;i+= 2)
	{

		QDomAttr    attrib;

		QString attribName  = list.at(i);
		QString attribValue = list.at(i+1);

		attrib = doc.createAttribute(attribName);
		attrib.setValue(QString("%1").arg(attribValue));
		element.setAttributeNode(attrib);

	}

}

 QDomElement DSE_XmlSaxHandler::createNode(QDomDocument &doc,QDomElement &parentElement,QString nodeName,QStringList &list  )
 {

    QDomElement element;
	QDomAttr    attrib;

	//创建节点
	element = doc.createElement(nodeName);
	parentElement.appendChild(element); 

	//创建节点的属性
	for (int i = 0 ;i < list.count() ; i+=2)
	{

		QString attribName  = list.at(i);
		QString attribValue = list.at(i+1);

		attrib = doc.createAttribute(attribName);
		attrib.setValue(QString("%1").arg(attribValue));
		element.setAttributeNode(attrib);

	}

	return element;

 }


 bool DSE_XmlSaxHandler::startElement(const QString &namespaceURI ,
                                const QString &localName,
                                const QString &qName,
                                const QXmlAttributes &attributes)
 {
    
	 return analystStartEmelent(namespaceURI,localName,qName,attributes);
 }

 bool DSE_XmlSaxHandler::endElement(const QString  &namespaceURI ,
                              const QString &localName ,
                              const QString &qName)
 {
    return analystEndEmelent(namespaceURI,localName,qName);
 }

 bool DSE_XmlSaxHandler::characters(const QString &str)
 {
	return analystCharacters(str);

 }

 
 QString DSE_XmlSaxHandler::errorString() const
 {
     return "";
 }

 bool DSE_XmlSaxHandler::fatalError(const QXmlParseException &exception)
 {

	  QString  str = QObject::tr("Parse error at line %1, column %2:\n"
                                          "%3")
                              .arg(exception.lineNumber())
                              .arg(exception.columnNumber())
                              .arg(exception.message());

	  ////qDebug() << str;

	   return false;

 }


 void DSE_SysImpl::initVars()
 {

     m_bDomain = false;
     m_bSta = false;
     m_bEqu = false;


 }

 int DSE_SysImpl::read(char *fileName,
				  char* err )
 {

      initVars();
	  clear();
	  return this->readFile(fileName,err);

 }


 int DSE_SysImpl::write(char *fileName,
			    char* err )
 {


	  QDomDocument doc;

     //创建xml节点
	 createXMlNode(doc);

	 //形成xml文件
	 return writeFile(doc,fileName);

 }

 void DSE_SysImpl::clear()
 {

	 for (int ll = 0 ; ll < m_sysNode.domainList.size() ; ll++)
	 {

		 PDOMAINNODE pDomain =  m_sysNode.domainList[ll];

		  for (int i = 0 ; i < pDomain->staList.size() ;i++)
		 {
			 PSTANODE pSta = pDomain->staList[i];

			 for (int j = 0 ; j < pSta->iedList.size() ; j++)
			 {

				 PEQUIPNODE pEqu = pSta->iedList[j];

				 delete pEqu;
			 }
			 pSta->iedList.clear();
			 delete pSta;
		 }

	    pDomain->staList.clear();

		delete pDomain;
	 }

	 m_sysNode.domainList.clear();
	 


	
	 
 }

 bool DSE_SysImpl::analystStartEmelent(const QString &namespaceURI, const QString &localName,
                       const QString &qName, const QXmlAttributes &attributes)
 {

	 if (qName == "SYS")
	 {

		 sprintf(m_sysNode.name,"%s",attributes.value("name").toAscii().data());
		 sprintf(m_sysNode.defaultSta,"%s",attributes.value("defaultSta").toAscii().data());
		 sprintf(m_sysNode.defaultEqu,"%s",attributes.value("defaultEqu").toAscii().data());
		 sprintf(m_sysNode.scl_fileName_modelCheck,"%s",attributes.value("scl_fileName_mCheck").toAscii().data());
	 }
	else if (qName == "DOMAIN")
	{
		m_bDomain = true;

		PDOMAINNODE pDomain = new DOMAINNODE();
		m_pDomainNode = pDomain;
		m_sysNode.domainList.push_back(pDomain);
		sprintf(pDomain->name,"%s",attributes.value("name").toAscii().data());		
	}
	 else if (qName == "STA")
	 {
		 m_bSta = true;

		 if (m_bDomain)
		 {
			PSTANODE pSta = new STANODE();
			m_pDomainNode->staList.push_back(pSta);
			m_pStaNode = pSta;

			sprintf(pSta->name,"%s",attributes.value("name").toAscii().data());	
		 }
		
	 }
	 else if (qName == "EQU")
	 {
		m_bEqu = true;

		 if (m_bDomain && m_bSta)
		 {
			PEQUIPNODE pEqu = new EQUIPNODE();
			m_pStaNode->iedList.push_back(pEqu);
		

			sprintf(pEqu->name,"%s",attributes.value("name").toAscii().data());	
			sprintf(pEqu->ip,"%s",attributes.value("ip").toAscii().data());
			pEqu->port  = attributes.value("port").toInt();
			pEqu->id    = attributes.value("id").toInt();
			pEqu->type    = attributes.value("type").toInt();
			pEqu->model    = attributes.value("model").toInt();
			sprintf(pEqu->scl.scl_file_name,"%s",attributes.value("scl_file_name").toAscii().data());
			sprintf(pEqu->scl.ied_name,"%s",attributes.value("ied_name").toAscii().data());
			sprintf(pEqu->scl.access_point_name,"%s",attributes.value("access_point_name").toAscii().data());
			pEqu->scl.enabled = attributes.value("scl_enabled").toInt();
			
		 }

	 }
	 else if (qName == "USERROOT")
	 {

		 sprintf(m_userNode.currebtUser.name,"%s",attributes.value("UserName").toAscii().data());
		 sprintf(m_userNode.currebtUser.password,"%s",attributes.value("Password").toAscii().data());
		 sprintf(m_userNode.currebtUser.grp,"%s",attributes.value("UserGrp").toAscii().data());

	 }
	 else if (qName == "ENVIR")
	 {

		 sprintf(m_envirNode.cfgDir,"%s",attributes.value("CfgDir").toAscii().data());
		 sprintf(m_envirNode.dataDir,"%s",attributes.value("DataDir").toAscii().data());
		 sprintf(m_envirNode.proDir,"%s",attributes.value("ProDir").toAscii().data());
		 sprintf(m_envirNode.sclPath,"%s",attributes.value("sclPath").toAscii().data());
		 sprintf(m_envirNode.waveFileName,"%s",attributes.value("waveFileName").toAscii().data());
		 m_envirNode.eventCount = attributes.value("EventCount").toInt();
		 m_envirNode.circleInterval = attributes.value("CircleInterval").toInt();
		 m_envirNode.bBeckonWave    = attributes.value("BeckonWave").toInt();
		 m_envirNode.bSaveToFile    = attributes.value("SaveToFile").toInt();
		
		 

		 //缺省配置
		 if (strlen(m_envirNode.cfgDir) == 0)
		 {
			 sprintf(m_envirNode.cfgDir,"%s/config",QApplication::applicationDirPath().toAscii().data());
		 }

		  if (strlen(m_envirNode.dataDir) == 0)
		 {	
			 sprintf(m_envirNode.dataDir,"%s/data",QApplication::applicationDirPath().toAscii().data());
		 }
		   if (strlen(m_envirNode.proDir) == 0)
		 {
			 sprintf(m_envirNode.proDir,"%s",QApplication::applicationDirPath().toAscii().data());
		 }

		 if (m_envirNode.eventCount == 0)
			 m_envirNode.eventCount = 200;

		 if ( m_envirNode.circleInterval == 0)
		 {
			 m_envirNode.circleInterval = 1000;
		 }

	 }
	 else if (qName == "USER")
	 {

		 PSINGLEUSER pUser = new SINGLEUSER();
		 m_userNode.userList.push_back(pUser);
		 sprintf(pUser->name,"%s",attributes.value("UsrName").toAscii().data());
		 sprintf(pUser->password,"%s",attributes.value("Password").toAscii().data());
		 sprintf(pUser->grp,"%s",attributes.value("UserGrp").toAscii().data());

	 }
	 else if (qName == "GRP")
	 {

		 PUSERGRP pGrp = new USERGRP();
		 m_userNode.userGrpList.push_back(pGrp);
		 sprintf(pGrp->grpName,"%s",attributes.value("GrpName").toAscii().data());
	 }


	 return true;
 }


  bool DSE_SysImpl::analystEndEmelent(const QString &namespaceURI, const QString &localName,
						const QString &qName)
  {


	 if (qName == "DOMAIN")
	{
		m_bDomain = false;
		
	}
	 else if (qName == "STA")
	 {
		m_bSta = false;

	 }
	 else if (qName == "EQU")
	 {
		m_bEqu = false;

	 }

	  return true;
  }


  bool DSE_SysImpl::analystCharacters(const QString &str)
  {


	  return true;
  }


  void DSE_SysImpl::createXMlNode(QDomDocument &doc)
  {

	  QDomText text;
	  QStringList strList;
	
	  QDomProcessingInstruction instruction;

	  instruction = doc.createProcessingInstruction("xml","version=\'1.0\' encoding=\'GBK\'");
	  doc.appendChild(instruction);

	  QDomElement rootElem = doc.createElement("SYS");
	  doc.appendChild(rootElem);

	  strList.clear();
	  strList 	  << QString("name")			    << QString("%1").arg(m_sysNode.name)
	              << QString("defaultSta")			<< QString("%1").arg(m_sysNode.defaultSta)
				  << QString("defaultEqu")			<< QString("%1").arg(m_sysNode.defaultEqu)
				   << QString("scl_fileName_mCheck")<< QString("%1").arg(m_sysNode.scl_fileName_modelCheck);
	  createAttrib(doc,rootElem,strList);


	  for (int ll = 0 ; ll < m_sysNode.domainList.size() ;ll++)
	  {
		  PDOMAINNODE pDomain = m_sysNode.domainList[ll];

		  strList.clear();
		  strList  << QString("type")		    << QString("%1").arg(TY_SYS) 
				   << QString("name")			<< QString("%1").arg(pDomain->name);
		  QDomElement domElem = createNode(doc,rootElem,QString("DOMAIN"),strList);


		  for (int i =0 ; i < pDomain->staList.size() ; i++)
		  {

			  PSTANODE pSta = pDomain->staList[i];

			  strList.clear();
			  strList  << QString("type")	    << QString("%1").arg(TY_STA) 
						 << QString("name")    << QString("%1").arg(pSta->name);

			 QDomElement elem2 = createNode(doc,domElem,QString("STA"),strList);


			 for (int j = 0 ; j < pSta->iedList.size() ; j++)
			 {

				 PEQUIPNODE pEqu = pSta->iedList[j];


				 strList.clear();
				 strList  << QString("type")          << QString("%1").arg(TY_EQP) 
						  << QString("name")          << QString("%1").arg(pEqu->name)
						  << QString("id")            << QString("%1").arg(pEqu->id)
						  << QString("ip")            << QString("%1").arg(pEqu->ip)
						  << QString("port")          << QString("%1").arg(pEqu->port)
						  << QString("model")         << QString("%1").arg(pEqu->model)
						  << QString("scl_file_name") << QString("%1").arg(pEqu->scl.scl_file_name)
						  << QString("ied_name") << QString("%1").arg(pEqu->scl.ied_name)
						  << QString("access_point_name") << QString("%1").arg(pEqu->scl.access_point_name)
						  << QString("scl_enabled") << QString("%1").arg(pEqu->scl.enabled);

				 createNode(doc,elem2,QString("EQU"),strList);

			 }
		  }

	  }

	  //环境节点
	 strList.clear();
	 strList  << QString("CfgDir")		    << QString("%1").arg(m_envirNode.cfgDir) 
			  << QString("DataDir")	        << QString("%1").arg(m_envirNode.dataDir)
			  << QString("proDir")	        << QString("%1").arg(m_envirNode.proDir)
			  << QString("eventCount")	    << QString("%1").arg(m_envirNode.eventCount)
			  << QString("CircleInterval")	<< QString("%1").arg(m_envirNode.circleInterval)
			  << QString("BeckonWave")	    << QString("%1").arg(m_envirNode.bBeckonWave)
			  << QString("SaveToFile")	    << QString("%1").arg(m_envirNode.bSaveToFile)
			  << QString("sclPath")	        << QString("%1").arg(m_envirNode.sclPath)
			  << QString("waveFileName")    << QString("%1").arg(m_envirNode.waveFileName);
			 
	  QDomElement domElem = createNode(doc,rootElem,QString("ENVIR"),strList);


	  //用户设置
	  strList.clear();
	  strList  << QString("UserName")	   << QString("%1").arg(m_userNode.currebtUser.name) 
			   << QString("Password")	   << QString("%1").arg(m_userNode.currebtUser.password)
			   << QString("UserGrp")	   << QString("%1").arg(m_userNode.currebtUser.grp);

	   QDomElement userRootElem = createNode(doc,rootElem,QString("USERROOT"),strList);

	   //用户列表
	   for (int i = 0 ; i < m_userNode.userList.size() ; i++)
	   {
		   PSINGLEUSER pUser = m_userNode.userList[i];

		   strList.clear();
		   strList  << QString("UsrName")	   << QString("%1").arg(pUser->name) 
		  		    << QString("Password")	   << QString("%1").arg(pUser->password)
					<< QString("UserGrp")	   << QString("%1").arg(pUser->grp);

		   QDomElement userElem = createNode(doc,rootElem,QString("USER"),strList);

	   }


	   //用户组
	   for (int i = 0 ; i < m_userNode.userGrpList.size() ; i++)
	   {
		   PUSERGRP pGrp = m_userNode.userGrpList[i];

		   strList.clear();
		   strList  << QString("GrpName")	   << QString("%1").arg(pGrp->grpName);

		   createNode(doc,rootElem,QString("GRP"),strList);

	   }

  }