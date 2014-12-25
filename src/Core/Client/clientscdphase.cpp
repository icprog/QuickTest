#include "clientscdphase.h"
#include <QtDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>

/***************************************ClientDSE_ScdManagerImpl begin**********************************************/

void ClientDSE_ScdManagerImpl::initScdVars()
{	
	m_fileFlag = FL_SCD;
	bWrite = false;

	//Substation节点
	bSubstation = false;
	strSubstation = "Substation";
	m_qNameMap.insert(strSubstation,1);

	//Communication大的节点  ======================================begin
	//Communication
	bCommunication = false;
	strCommunication = "Communication";
	m_qNameMap.insert(strCommunication,2);
	//SubNetwork
	bSubNetwork = false;
	strSubNetwork = "SubNetwork";
	m_qNameMap.insert(strSubNetwork,3);
	//ConnectedAP
	bConnectedAP = false;
	strConnectedAP = "ConnectedAP";
	m_qNameMap.insert(strConnectedAP,4);
	//GSE
	bGSE = false;
	strGSE = "GSE";
	m_qNameMap.insert(strGSE,5);
	bSMV = false;
	strSMV = "SMV";
	m_qNameMap.insert(strSMV,6);
	//p
	bP = false;
	strP = "P";
	m_qNameMap.insert(strP,7);
	strMacAddr = "MAC-Address";
	m_qNameMap.insert(strMacAddr,8);
	bMacAddr = false;
	strAppId = "APPID";
	m_qNameMap.insert(strAppId,9);
	bAppId = false;
	strIp = "IP";
	m_qNameMap.insert(strIp,10);
	bIp = false;
	bMinTime = false;
	strMinTime = "MinTime";
	bMaxTime = false;
	strMaxTime = "MaxTime";
	strIPSUBNET = "IP-SUBNET";
	bIPSUBNET = false;
	//Communication end

	//IED  节点 begin
	strIed = "IED";
	m_qNameMap.insert(strIed,19);
	bIed = false;
	strAccessPoint = "AccessPoint";
	m_qNameMap.insert(strAccessPoint,20);
	bAccessPoint = false;
	strService = "Server";
	m_qNameMap.insert(strService,21);
	bService = false;
	strLDevice = "LDevice";
	m_qNameMap.insert(strLDevice,22);
	bLdevice = false;
	strLN0 = "LN0";
	m_qNameMap.insert(strLN0,23);
	bLn0 = false;
	strDataSet = "DataSet";
	m_qNameMap.insert(strDataSet,24);
	bDataSet = false;
	strFcda = "FCDA";
	m_qNameMap.insert(strFcda,25);
	bFcda = false;
	bInputs = false;
	strGSEControl = "GSEControl";
	m_qNameMap.insert(strGSEControl,26);
	bGSEControl = false;
	strSampledValueControl = "SampledValueControl";
	m_qNameMap.insert(strSampledValueControl,27);
	bSampledValueControl = false;
	strReportControl = "ReportControl";
	m_qNameMap.insert(strReportControl,28);
	strSettingControl = "SettingControl";
	m_qNameMap.insert(strSettingControl,29);
	strRptEnabled = "RptEnabled";
	m_qNameMap.insert(strRptEnabled,30);

	strLN = "LN";
	m_qNameMap.insert(strLN,31);
	bLn = false;
	strDoi = "DOI";
	m_qNameMap.insert(strDoi,32);
	bDoi = false;
	m_qNameMap.insert("Inputs",33);
	m_qNameMap.insert("ExtRef",34);
	m_qNameMap.insert("SDI",35);
	m_qNameMap.insert("DAI",36);
	m_qNameMap.insert("Val",37);
	m_qNameMap.insert("LNodeType",38);
	m_qNameMap.insert("DO",39);
	m_qNameMap.insert("DOType",40);
	m_qNameMap.insert("SDO",41);
	m_qNameMap.insert("DA",42);
	m_qNameMap.insert("DAType",43);
	m_qNameMap.insert("BDA",44);

	strLN = "LN";
	bLn = false;
	strDoi = "DOI";
	bDoi = false;

	m_pLnType = NULL;
	m_pDoType = NULL;
	m_pFcda = NULL;
	m_pDaType = NULL;

	m_subNetList = NULL;
	m_pCustomSubNet = NULL;
	m_ied2 = NULL;

	m_fileProgress.fileLengthed = 0;
	m_fileProgress.fileTotalLen = 0;
	sprintf(m_fileProgress.destName,"scd文件进度指示");
	m_fileStep = 0;

	bScl = false;
	m_equipTotal = 0;

	m_msvgse = NULL;

	bMultiplier = false;
	bMax = false;        
	bMin = false;
	bUnit = false;
	bStep = false;
	m_bComLayer = true;
	if (m_decodeflag == DECODE_PT)
	{
		m_bComLayer = false;
	}

	m_pMulteiFcda = NULL;

	m_ied = NULL;
	m_strMultiplier = "";


}

void ClientDSE_ScdManagerImpl::initXmlVars()
{
	//m_netCfg = NULL;
	m_ipCount = 0;

	m_bNetWork = false;
	m_bNetCfg = false;
	m_bEthCfg = false;
	m_bSubNet = false;
	m_bIed = false;
	m_bSmv = false; 
	m_curInfo = false;
	m_bChnInfo = false;
	m_bGoose = false;
	m_mmsInfo = NULL;

}

void ClientDSE_ScdManagerImpl::clearDataSetList()
{

	for (int i = m_dataSetList.count() ; i>0 ; i--)
	{

		PDATASET dataSet = m_dataSetList.at(i-1);

		for (int j = dataSet->fcdaList.count();j >0 ;j--)
		{
			PFCDA fcda = dataSet->fcdaList.at(j-1);

			delete fcda;
		}

		dataSet->fcdaList.clear();
		delete dataSet;

	}

	m_dataSetList.clear();

}

t_int ClientDSE_ScdManagerImpl::readScd(const t_char        *fileName,
										std::vector<PSUBNET>    &subNetList,
										SUBSTA               &subSta,
										t_uint8  flag,
										t_char* err )
{
	m_decodeflag = flag;
	initScdVars();

	m_fileFlag = FL_SCD;


	m_subNetList = &subNetList;
	m_subSta    = &subSta;

	QDateTime dD = QDateTime::currentDateTime();

	QString  str= dD.toString("yyyy/MM/dd HH:mm:ss.zzz");

	printf("readFile:%s begn\n",str.toAscii().data());
	t_int ret = readFile(fileName,err);

	dD = QDateTime::currentDateTime();
	str= dD.toString("yyyy/MM/dd HH:mm:ss.zzz");
	printf("readFile:%s end\n",str.toAscii().data());

	if (m_decodeflag == DECODE_COMMLAYER)
	{
		//makeScdIp();
		makeScdInf();
	}
	else if (m_decodeflag == DECODE_PT)
	{	
		//makeScdInf1();
		//makeScdInf2();
	}
	else 
	{
		//makeScdInf();		
	}


	//清除解析scd所分配的内存
	clearScdVars();



	//设置文件长度
	m_fileProgress.fileLengthed = m_fileProgress.fileTotalLen;

	CMD_TYPE_DEF::CMD cmd;

	cmd.head.cmd = CMD_TYPE_DEF::CMD_FILE_PROGRESSS;
	memcpy(cmd.data.str,&m_fileProgress,sizeof(CMD_TYPE_DEF::FILE_PROGRESS));
	if (m_routine)
	{
		//回调，传送的文件已经接收的文件长度
		m_routine(m_param,cmd);
	}


	return ret;
}


t_int ClientDSE_ScdManagerImpl::write(const t_char          *fileName,
									  std::vector<PSUBNET>    &subNetList,
									  SUBSTA               &subSta,
									  t_char* err  )
{

	m_fileFlag = FL_XML; 
	m_subNetList = &subNetList;
	m_subSta    = &subSta;

	QDomDocument doc;

	createXMlNode(doc);
	//形成xml文件
	return writeFile(doc,fileName);

}

t_int ClientDSE_ScdManagerImpl::read(const t_char          *fileName,
									 std::vector<PSUBNET>    &subNetList,
									 SUBSTA               &subSta,
									 t_char* err )
{
	initXmlVars();
	m_fileFlag = FL_XML;  
	m_subNetList = &subNetList;
	m_subSta    = &subSta;

	return readFile(fileName,err);

}

void ClientDSE_ScdManagerImpl::clearScdVars()
{
	m_fileFlag = FL_XML; 
	u8SubNetwork = 0;
	clearDataSetList();
	clearStation();

	m_doMap.clear();
	m_sdoMap.clear();
	m_lnTypeMap.clear();


}



bool ClientDSE_ScdManagerImpl::analystCharacters(const QString &str)
{
	if (bMacAddr && m_msvgse)
	{
		bMacAddr = false;
		m_msvgse->macAddr = str;

	}
	else if (bAppId && m_msvgse)
	{
		bAppId = false;
		if (m_msvgse != NULL)
			m_msvgse->appId = str;

	}
	else if (bIp)
	{
		bIp = false;
		if (m_mmsInfo1 != NULL)
			sprintf( m_mmsInfo1->ip,"%s",str.toAscii().data());

	}
	else if (bIPSUBNET)
	{
		bIPSUBNET = false;

		if (m_mmsInfo1 != NULL)
			sprintf( m_mmsInfo1->mask,"%s",str.toAscii().data());
	}

	//if (m_decodeflag == DECODE_COMMLAYER)
	//  return true;

	if (m_bComLayer == true)
		return true;


	if (bMinTime)
	{
		bMinTime = false;
		if (m_msvgse != NULL)
			m_msvgse->t1 = str.toInt();

	}
	else if (bMaxTime)
	{
		if (m_msvgse != NULL)
			m_msvgse->t0 = str.toInt();
		bMaxTime = false;

	}
	else if (bMax)
	{
		setMaxMinUnitStepVal(m_fcda,1,str);
		bMax = false;
	}
	else if (bMin)
	{
		setMaxMinUnitStepVal(m_fcda,2,str);
		bMin = false;
	}
	else if (bUnit)
	{

		setMaxMinUnitStepVal(m_fcda,4,str);
		bUnit = false;
	}
	else if (bMultiplier)
	{ 

		if (m_pMulteiFcda == NULL)
		{
			//setMaxMinUnitStepVal(m_fcda,5,str);
			//qDebug() << "dkk" << str << "fff";
			m_strMultiplier = str;

		}
		else
		{
			t_char strMultiplier[100];
			memset(strMultiplier,0x00,100);

			QString strUnit= m_pMulteiFcda->unitVal;
			t_int val = str.toInt();
			if ((val == 0) && (str != "0"))
			{		
				sprintf(strMultiplier,"%s",str.toAscii().data());
				m_pMulteiFcda->unitVal = QString("%1%2").arg(strMultiplier).arg(strUnit);
			}
			else
			{
				getMultiplier(val,strMultiplier);

			} 
		}


		m_pMulteiFcda = NULL;

		bMultiplier = false;
	}
	else if (bStep)
	{
		setMaxMinUnitStepVal(m_fcda,3,str);
		bStep = false;
	}

	return true;
}

void   ClientDSE_ScdManagerImpl::startParseScdFile(const QString &namespaceURI, const QString &localName,
												   const QString &qName, const QXmlAttributes &attributes)
{   

	int qNameIndex = m_qNameMap.value(qName);

	//if (m_decodeflag == DECODE_COMMLAYER)
	switch(qNameIndex)
	{
	case 1:  //strSubstation
		{
			bSubstation = true;

			m_sta.name = attributes.value("name");
			m_sta.desc = attributes.value("desc");

		}
		break;
	case 2:
		{
			bCommunication = true;
		}
		break;
	case 3:  //strSubNetwork
		{
			bSubNetwork = true;

			if (bCommunication)
			{
				QString name = attributes.value("name");
				QString type = attributes.value("type");

				bWrite = true;
				u8SubNetwork++;

				CUSTOM_SUBNET *pSubNet = new CUSTOM_SUBNET();
				m_pCustomSubNet = pSubNet;
				m_sta.subNetList.push_back(pSubNet);
				sprintf(m_pCustomSubNet->name,"%s",name.toAscii().data());
				sprintf(m_pCustomSubNet->type,"%s",type.toAscii().data());

			}


		}
		break;
	case 4:      //strConnectedAP
		{

			QString strapName =attributes.value("apName");

			//20131125 begin
			//if ( bSubNetwork && bWrite && strapName.contains("S") )  
			//{
			//	    bConnectedAP = true;
			//		INTERNAL_IED *ied = new INTERNAL_IED;
			//		m_ied2 = ied;
			//		m_equipTotal++;
			//		ied->iedName = attributes.value("iedName");
			//		ied->desc = attributes.value("desc");
			//		ied->apName = attributes.value("apName");	
			//添加装置
			//		if (m_pCustomSubNet)
			//			m_pCustomSubNet->iedList.push_back(ied);
			//}
			//20131125 end

			//20131125 add
			bConnectedAP = true;
			if ( bSubNetwork && bWrite)
			{
				m_ied2Name = attributes.value("iedName");
				m_ied2Desc = attributes.value("desc");
				m_ied2ApName = attributes.value("apName");	

			}


		}
		break;
	case 5:   //strGSE
		{
			bGSE = true;

			if (bSubNetwork && bWrite && bConnectedAP)
			{
				SMVGSE1 *msvgse = new SMVGSE1;
				m_msvgse = msvgse;
				msvgse->control = GSMV_TYPE_GOOSE;
				msvgse->cbName = attributes.value("cbName");
				msvgse->ldInst = attributes.value("ldInst");

				if (m_ied2)
				{
					//添加goose
					m_ied2->smvGseList.append(msvgse);
				}

			}

		}
		break;

	case 6:  //strSMV
		{
			bSMV = true;

			if (bSubNetwork && bWrite && bConnectedAP)
			{
				SMVGSE1 *msvgse = new SMVGSE1;
				m_msvgse = msvgse;
				msvgse->control = GSMV_TYPE_SMV;
				msvgse->cbName = attributes.value("cbName");
				msvgse->ldInst = attributes.value("ldInst");

				if (m_ied2)
				{
					//添加goose
					m_ied2->smvGseList.append(msvgse);
				}
				else
				{
					delete msvgse;
					m_msvgse = NULL;

				}


			}
		}
		break;

	case 7:    //strP
		{
			bP = true;

			if ((bSubNetwork && bWrite && bConnectedAP) && (bGSE || bSMV))
			{ 
				QString str =  attributes.value("type");

				if (str == strMacAddr)
				{

					bMacAddr = true;
				}
				else if (str == strAppId)
				{
					bAppId = true;


				}

			}
			//ip地址和掩码
			else if(bSubNetwork && bWrite && bConnectedAP)
			{
				QString str =  attributes.value("type");

				if (str == strIp)
				{		
					bIp = true;

					//20131125 begin
					INTERNAL_IED *ied = new INTERNAL_IED;
					m_ied2 = ied;

					m_equipTotal++;

					ied->iedName = m_ied2Name;
					ied->desc = m_ied2Desc;
					ied->apName = m_ied2ApName;


					//添加装置
					if (m_pCustomSubNet)
						m_pCustomSubNet->iedList.push_back(ied);
					////20131125 end

					PMMSINFO pMms = new MMSINFO();
					m_mmsInfo1 = pMms;
					if (m_ied2)
					{
						m_ied2->mmsList.push_back(pMms);	
						sprintf(pMms->apName,"%s",m_ied2->apName.toAscii().data());  //添加访问点

					}
				}
				else if (str == strIPSUBNET)
				{
					bIPSUBNET = true;
				}

			}
		}
		break;
	case 16:  //strMinTime
		{
			bMinTime = true;


		}
		break;
	case 17:  //strMaxTime
		{
			bMaxTime = true;

		}
		break;



	case 19: //strIed   <IED name="IXL2201A" desc="220kV马家1线智能终端A" manufacturer="NARITECH" configVersion="V1.0">
		{
			bIed = true;

			QString iedName = attributes.value("name");

			//添加装置的描述和制造厂家

			PINTERNAL_IED1 pIed1 = new INTERNAL_IED1();
			m_ied = pIed1;
			m_sta.iedList.append(pIed1);

			pIed1->desc = attributes.value("desc");
			pIed1->manufacturer = attributes.value("manufacturer");
			pIed1->iedName = attributes.value("name");



		}
		break;

	case 20:  //strAccessPoint
		{

			QString name = attributes.value("name");
			bAccessPoint = false;

			// if ((name.contains("G")) || (name.contains("M")) )  //以不以这个名称区分goose还是smv?
			//if (name.contains("S")) 
			{
				bAccessPoint = true;
				bGSEControl = false;
			}

		}
		break;

	case 21:  //strService
		{
			bService = true;
		}
		break;
	case 22:  //strLDevice
		{
			bLdevice = true;

			m_inst = attributes.value("inst");

		}
		break;
	case 23:  //strLN0
		{
			bLn0 = true;
			//清除ln0节点下的所有数据集
			clearDataSetList();
			m_dataSet = NULL;
			m_smvgse = NULL;
			m_doMap.clear();
			m_sdoMap.clear();
			m_lnTypeMap.clear();

			//20140221 加 
			m_fcda.lnDesc  = "";
			m_fcda.prefix  = "";
			m_fcda.lnInst  = "";
			m_fcda.lnClass = "";
			m_fcda.lnType  =  attributes.value("lnType");
			m_fcda.ldInst  = ""; 
		}
		break;
	case 24:   //strDataSet
		{


			if (bIed && bAccessPoint && bService && bLn0 )
			{

				QString name = attributes.value("name");
				QString desc = attributes.value("desc");

				t_int8 attrbId = -1;
				if (getContrlAttrib(name,attrbId)==0)
				{
					PDATASET dataSet = new DATASET;
					dataSet->name = name;
					dataSet->desc = desc;
					m_dataSet = dataSet;

					/* m_dataSetList.append(dataSet);*/

					bDataSet = true;

					PSMVGSE smvgse = new SMVGSE();//findSMVGse(name,GSMV_TYPE_GOOSE);
					if (smvgse)
					{
						smvgse->control  = attrbId;   //填的是属性id，以前填的sv goose
						smvgse->cbName   = name;
						/*	 smvgse->sgAppid = appID;*/
						smvgse->datSet  = name;
						/* smvgse->confRev = confRev;*/
						smvgse->desc    = name;
						smvgse->inst    = m_inst;


						m_smvgse = smvgse;
						m_ied->smvGseList.push_back(smvgse);
						smvgse->dataSetList.push_back(dataSet);
					}



				}

			}

		}
		break;
	case 25:
		{

			if (bIed && bAccessPoint && bService && bLn0  && bDataSet)
			{
				bFcda = true;

				QString doName   = attributes.value("doName");
				QString daName   = attributes.value("daName");
				QString ldInst   = attributes.value("ldInst");
				QString lnClass  = attributes.value("lnClass");
				QString prefix   = attributes.value("prefix");
				QString fc       = attributes.value("fc");
				QString lnInst   = attributes.value("lnInst");


				if ( (daName == QString("q") ) || (daName == QString("t")))
				{

				}
				else
				{
					PFCDA fcda = new FCDA;
					m_pFcda = fcda;				 
					m_pFcda->dataType.fromatValue = GSDATA_TYPE::GTE_STRVAL;
					m_pFcda->dataType.bStruct  = false;  
					fcda->doName   = attributes.value("doName");
					fcda->daName   = attributes.value("daName");
					fcda->ldInst   = attributes.value("ldInst");
					fcda->lnClass  = attributes.value("lnClass");
					fcda->prefix   = attributes.value("prefix");
					fcda->fc       = attributes.value("fc");
					fcda->lnInst   = attributes.value("lnInst");
					m_dataSet->fcdaList.append(fcda);

					//建立映射
					QString ref = QString("%1%2/%3%4%5$%6")
						.arg(m_ied->iedName)
						.arg(fcda->ldInst)
						.arg(fcda->prefix)
						.arg(fcda->lnClass)
						.arg(fcda->lnInst)
						.arg(fcda->doName);							  
					ref.replace(".","$");

					//2014-5-8
					if (fcda->doName.contains(".") == true)  //do上包含sdo  
					{
						m_sdoMap.insert(ref,fcda);
					}
					else
					{
						m_doMap.insert(ref,fcda);
					}

					if (fcda->lnClass == "LLN0")
					{
						fcda->lnType =  m_fcda.lnType;

					}
					else
					{
						ref = QString("%1$%2$%3$%4")
							.arg(fcda->prefix)
							.arg(fcda->lnClass)
							.arg(fcda->ldInst)
							.arg(fcda->lnInst);

						m_lnTypeMap.insert(ref,fcda);

					}



				}	
			}
		}
		break;
	case 28:  //strReportControl
		{
			QString datSet = attributes.value("datSet");

			PSMVGSE pSmvgse = findSMVGse(datSet,0,m_inst);

			if (pSmvgse)
			{
				m_smvgse = pSmvgse;

				sprintf( pSmvgse->reportCtrl.ld,"%s%s",m_ied->iedName.toAscii().data(),m_inst.toAscii().data());
				sprintf( pSmvgse->reportCtrl.name,"%s",attributes.value("name").toAscii().data());
				sprintf( pSmvgse->reportCtrl.datSet,"%s",datSet.toAscii().data());
				sprintf( pSmvgse->reportCtrl.rptID,"%s",attributes.value("rptID").toAscii().data());
			}
		}
		break;
	case 30:  //strRptEnabled
		{
			if (m_smvgse)
			{
				m_smvgse->reportCtrl.max = attributes.value("max").toInt();

			}
		}
		break;
	case 29:  //strSettingControl
		{
			m_settingCtrl.actSG    = attributes.value("actSG").toInt();                     
			m_settingCtrl.numOfSGs = attributes.value("numOfSGs").toInt();

		}
		break;
	case 31:  //strLN
		{

			if (bIed && bAccessPoint && bService )
			{
				bLn = true;
				QString prefix = attributes.value("prefix");
				QString desc = attributes.value("desc");
				QString inst = attributes.value("inst");
				QString lnClass = attributes.value("lnClass");
				QString lnType = attributes.value("lnType");
				m_fcda.doName = "";
				m_fcda.daName = "";

				m_fcda.lnDesc  = desc;
				m_fcda.prefix  = prefix;
				m_fcda.lnInst  = inst;
				m_fcda.lnClass = lnClass;
				m_fcda.lnType  = lnType;

				//取得goose的lnType，lnClass

				//设置数据点的lnType
				setFcdaLnType( prefix,lnClass,inst, lnType);

				//if (m_decodeflag == DECODE_PT)
				{
					PLNODE_TYPE pLNType = findLnType(lnType,lnClass);

					if (!pLNType)
					{
						pLNType = new LNODE_TYPE();
						//m_sta.lnTypeList.append(pLNType);
						m_sta.lnTypeListMap.insert(m_fcda.lnType,pLNType);
						sprintf(pLNType->id,"%s",m_fcda.lnType.toAscii().data());
						sprintf(pLNType->lnClass,"%s",m_fcda.lnClass.toAscii().data());

					}
				}

			}
		}
	case 32:
		{	
			/* if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/

			bDoi = false;

			if (bIed && bAccessPoint && bService)
			{
				bDoi = true;
				QString desc = attributes.value("desc");
				QString name = attributes.value("name");
				m_fcda.doName = name;
				m_strDoi = name;


				if (bLn0 )
				{

					m_fcda.lnDesc  = desc;
					m_fcda.prefix  = "";
					m_fcda.lnInst  = "";
					m_fcda.lnClass = "LLN0";
					m_fcda.ldInst = m_inst;
					// 2014-5-8暂时不用
					//setlnClassLLN0ChinName(m_fcda,desc);

					QString ref = QString("%1%2/%3%4%5$%6")
						.arg(m_ied->iedName)
						.arg(m_fcda.ldInst)
						.arg(m_fcda.prefix)
						.arg(m_fcda.lnClass)
						.arg(m_fcda.lnInst)
						.arg(m_fcda.doName);							  
					ref.replace(".","$");

					PFCDA pFcda = m_doMap.value(ref);
					if (pFcda)
						pFcda->chnName = desc;

				}
				else
				{
					//设置对应的中文  2014-5-8暂时不用
					//setChinName(m_fcda,desc,m_fcda.doName,"");  

					//建立映射
					QString ref = QString("%1%2/%3%4%5$%6")
						.arg(m_ied->iedName)
						.arg(m_fcda.ldInst)
						.arg(m_fcda.prefix)
						.arg(m_fcda.lnClass)
						.arg(m_fcda.lnInst)
						.arg(m_fcda.doName);							  
					ref.replace(".","$");

					PFCDA pFcda = m_doMap.value(ref);
					if (pFcda)
						pFcda->chnName = desc;





				}

				strSDI = "";
				bSDI = false;
			}	   
		}
		break;
	case 35://"SDI"
		{
			/*  if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/

			if (bDoi == true)
			{
				QString name = attributes.value("name");
				QString desc = attributes.value("desc");

				bSDI = true;
				strSDI = name;

				if (name == "phsA")
					QString str ="";

				bMax = false;
				bMin = false;
				bUnit = false;
				bStep = false;
				bMultiplier = false;

				QString  tmpdoName = QString("%1.%2").arg(m_strDoi).arg(name);	
				//设置对应的中文   2014-5-8暂时不用
				/* bool bRet = setSDIChinName(m_fcda,desc,tmpdoName);  
				if (bRet)
				{
				m_fcda.doName = tmpdoName;
				}*/


				//建立映射
				QString ref = QString("%1%2/%3%4%5$%6")
					.arg(m_ied->iedName)
					.arg(m_fcda.ldInst)
					.arg(m_fcda.prefix)
					.arg(m_fcda.lnClass)
					.arg(m_fcda.lnInst)
					.arg(tmpdoName);							  
				ref.replace(".","$");

				QList<PFCDA> list = m_sdoMap.values(ref);

				for (int i = 0 ;i < list.count() ; i++)
				{
					PFCDA pFcda = list.at(i);
					if (pFcda)
						pFcda->chnName = desc;
				}

			}	
		}
		break;
	case 36:  //"DAI"
		{
			/* if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/

			if (bDoi == true)
			{
				QString desc = attributes.value("desc");
				QString name = attributes.value("name");


				strDAI = name;

				m_fcda.daName = name;
				if (bSDI)
				{
					m_fcda.daName = QString("%1.%2").arg(strSDI).arg(name);
				}	

				if (desc != "")
				{
					//设置对应的中文
					//setChinName(m_fcda,desc,m_fcda.doName,m_fcda.daName);//暂时不处理doi //2014-5-04
				}

			}

		}
		break;
	case 37:  //"Val"
		{
			/* if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/

			if (bDoi == false)
				return ;

			if ( (strSDI == "maxVal") || (strSDI == "max")  || (strDAI == "maxVal"))
			{
				bMax = true;

			}
			else if ( (strSDI == "minVal") || (strSDI == "min") || (strDAI == "minVal"))
			{
				bMin = true;
			}

			else if ( (strSDI == "stepSize") ||  (strDAI == "stepSize"))
			{
				bStep = true;
			}

			if (strDAI == "SIUnit")
			{
				bUnit = true;
			}
			else if (strDAI == "multiplier")
			{
				bMultiplier = true;
			}


		}
		break;
	case 38:  //LN
		{
			/* if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/

			QString id = attributes.value("id");
			QString lnClass = attributes.value("lnClass");

			if (lnClass == "LLN0")   //如果是LLN0则添加，因为前面的ln0节点没有lnType
			{

				PLNODE_TYPE pLNType = new LNODE_TYPE();
				m_pLnType = pLNType;
				//m_sta.lnTypeList.append(pLNType);
				m_sta.lnTypeListMap.insert(id,pLNType);
				sprintf(pLNType->id,"%s",id.toAscii().data());
				sprintf(pLNType->lnClass,"%s",lnClass.toAscii().data());
				sprintf(pLNType->desc,"%s",attributes.value("desc").toAscii().data());
			}
			else
			{
				PLNODE_TYPE pLNType = findLnType(id,lnClass);
				m_pLnType = pLNType;

				if (pLNType)
				{
					sprintf(pLNType->desc,"%s",attributes.value("desc").toAscii().data());

				}
			}


		}
		break;
	case 39:  //"DO"
		{
			/* if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/

			if (m_pLnType)
			{
				PDO  pDo = new DO();
				//m_pLnType->doList.append(pDo);

				sprintf(pDo->type,"%s",attributes.value("type").toAscii().data());
				sprintf(pDo->desc,"%s",attributes.value("desc").toAscii().data());
				sprintf(pDo->name,"%s",attributes.value("name").toAscii().data());

				m_pLnType->doTypeListMap.insert(attributes.value("name"),pDo);
			}
		}
		break;
	case 40:   //"DOType"
		{
			/* if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/


			QString id = attributes.value("id");
			QString cdc = attributes.value("cdc");

			/* bool bRet  = findDo(id);*/

			m_pDoType = NULL;

			/* if (bRet)*/
			{
				DO_TYPE  *pDoType = new DO_TYPE();
				m_pDoType = pDoType;
				// m_sta.doTypeList.append(pDoType);
				m_pDoType = pDoType;
				sprintf(pDoType->id,"%s",id.toAscii().data());
				sprintf(pDoType->cdc,"%s",cdc.toAscii().data());

				m_sta.doTypeListMap.insert(id,pDoType);


			}
		}
		break;
	case 41:   //"SDO"
		{
			/* if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/

			QString  name = attributes.value("name");
			QString  type =  attributes.value("type");

			if (m_pDoType)
			{
				SDO  *pSdo = new SDO();

				sprintf(pSdo->name ,"%s",name.toAscii().data());
				sprintf(pSdo->type ,"%s",type.toAscii().data());
				//m_pDoType->sdoList.append(pSdo);
				m_pDoType->sdoListMap.insert(name,pSdo);

			}

		}
		break;
	case 42:  //"DA"
		{
			/*if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/

			QString  name = attributes.value("name");
			QString  bType= attributes.value("bType");
			QString  type =  attributes.value("type");
			QString  fc =  attributes.value("fc"); 

			if (m_pDoType)
			{
				DA  *pDa = new DA();

				sprintf(pDa->name ,"%s",name.toAscii().data());
				sprintf(pDa->bType ,"%s",bType.toAscii().data());
				sprintf(pDa->type ,"%s",type.toAscii().data());
				sprintf(pDa->fc ,"%s",fc.toAscii().data());
				m_pDoType->daList.append(pDa);


			}
		}
		break;
	case 43: // "DAType"
		{
			/*if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/

			QString id = attributes.value("id");


			/* bool bRet = findDa(id);

			m_pDaType = NULL;

			if (bRet )*/
			{
				PDA_TYPE pDaType = new DA_TYPE();
				m_pDaType = pDaType;
				sprintf(pDaType->id,"%s",id.toAscii().data());
				//m_sta.daTypeList.append(pDaType);
				m_sta.daTypeListMap.insert(id,pDaType);
			}

		}
		break;
	case 44:
		{
			/* if (m_decodeflag != DECODE_PT)
			{
			return ;
			}*/

			QString name = attributes.value("name");
			QString bType = attributes.value("bType");
			QString type = attributes.value("type");

			if (m_pDaType)
			{

				PBDA_TYPE pBdaType = new BDA_TYPE();
				//m_pDaType->bdaTypeList.append(pBdaType);

				sprintf(pBdaType->name,"%s",name.toAscii().data());
				sprintf(pBdaType->bType,"%s",bType.toAscii().data());
				sprintf(pBdaType->type,"%s",type.toAscii().data());

				m_pDaType->bdaTypeListMap.insert(name,pBdaType);
			}
		}
		break;

	}






}

void  ClientDSE_ScdManagerImpl::setMaxMinUnitStepVal(FCDA  fcda,t_uint type,QString strVal)
{

	if (!m_ied)
		return ;

	for (int i = 0 ; i < m_ied->smvGseList.count()  ; i++)
	{

		PSMVGSE smvgse = m_ied->smvGseList.at(i);

		if (smvgse == NULL)
			continue;

		for (int j = 0 ; j < smvgse->dataSetList.count() ; j++)
		{
			PDATASET dataSet = smvgse->dataSetList.at(j);

			for (int l = 0 ; l < dataSet->fcdaList.count() ; l++)
			{
				PFCDA fcda1 = dataSet->fcdaList.at(l);

				//写ai的名称
				if ((fcda1->prefix == fcda.prefix) && (fcda1->lnInst == fcda.lnInst) && (fcda1->lnClass == fcda.lnClass) && (fcda1->doName == fcda.doName) /* && (fcda1->daName == fcda.daName)*/)
				{
					switch(type)
					{
					case 1://最大值
						if (bMax)
							fcda1->maxVal = strVal;       //最大值
						break;
					case 2: //最小值
						if (bMin)
							fcda1->minVal = strVal;
						break;
					case 3://步长
						if (bStep)
							fcda1->stepVal = strVal;
						break;
					case 4://单位
						{				
							if (bUnit)
							{

								if (m_strMultiplier !="")
								{
									t_char strMultiplier[100];
									memset(strMultiplier,0x00,100);

									t_int val = m_strMultiplier.toInt();
									if ((val == 0) && (m_strMultiplier != "0"))
									{		
										sprintf(strMultiplier,"%s",m_strMultiplier.toAscii().data());
									}
									else
									{
										getMultiplier(val,strMultiplier);
									} 

									fcda1->unitVal = QString("%1").arg(strMultiplier);

									m_strMultiplier = "";
								}


								t_char unitStr[100];
								memset(unitStr,0x00,100);

								QString strUnit= fcda1->unitVal;

								t_int val = strVal.toInt();
								if (val == 0)
								{
									fcda1->unitVal = QString("%1%2").arg(strUnit).arg(strVal);
								}
								else
								{
									getUnit(strVal.toInt(),unitStr);
									fcda1->unitVal = QString("%1%2").arg(strUnit).arg(unitStr);
								}

								m_pMulteiFcda = fcda1;

							}
						}
						break;
					case 5://单位的倍乘
						{
							if (bMultiplier)
							{

								t_char strMultiplier[20];
								memset(strMultiplier,0x00,20);

								QString strUnit= fcda1->unitVal;
								t_int val = strVal.toInt();
								if (val == 0)
								{
									if (strVal != "0")
									{
										sprintf(strMultiplier,"%s",strVal.toAscii().data());
									}
								}
								else
								{
									getMultiplier(val,strMultiplier);

								}
								fcda1->unitVal = QString("%1%2").arg(strMultiplier).arg(strUnit);
							}
						}
						break;
					}
					return ;

				}			
			}
		}
	}


}

void   ClientDSE_ScdManagerImpl::startParseXmlFile(const QString &namespaceURI, const QString &localName,
												   const QString &qName, const QXmlAttributes &attributes)
{
	if (qName == "SubSta")
	{

		if (m_subSta)
		{
			strcpy(m_subSta->name,attributes.value("name").toAscii().data());

		}

	}
	else if (qName == "SubNet")
	{

		m_bSubNet = true;
		PSUBNET pSubNet = new SUBNET();
		m_subNet = pSubNet;

		strcpy(pSubNet->type,attributes.value("type").toAscii().data());
		strcpy(pSubNet->name,attributes.value("name").toAscii().data());
		strcpy(pSubNet->desc,attributes.value("desc").toAscii().data());

		m_subNetList->push_back(pSubNet);
	}
	else if(qName == "Ied")
	{
		m_bIed = true;

		if (m_bSubNet)
		{
			PIED pIed = new IED();
			m_ied1 = pIed;
			//添加ied
			m_subNet->iedList.push_back(pIed);
			strcpy(pIed->type,attributes.value("type").toAscii().data());
			strcpy(pIed->name,attributes.value("name").toAscii().data());
			strcpy(pIed->desc,attributes.value("desc").toAscii().data());
			strcpy(pIed->manufacturer,attributes.value("manufacturer").toAscii().data());
			pIed->settingCtrl.actSG = attributes.value("actSG").toInt();
			pIed->settingCtrl.numOfSGs = attributes.value("numOfSGs").toInt();

		}
	}
	else if (qName == "Mms")
	{

		PMMSINFO pMms = new MMSINFO();
		m_ied1->mmsList.push_back(pMms);
		strcpy(pMms->name,attributes.value("name").toAscii().data());
		strcpy(pMms->desc,attributes.value("desc").toAscii().data());
		strcpy(pMms->ip,attributes.value("ip").toAscii().data());
		strcpy(pMms->mask,attributes.value("mask").toAscii().data());
		strcpy(pMms->apName,attributes.value("apName").toAscii().data());
		strcpy(pMms->destIp,attributes.value("destIp").toAscii().data());
		strcpy(pMms->destMask,attributes.value("destMask").toAscii().data());
		pMms->ethIndex = attributes.value("ethIndex").toInt();
		pMms->appid = attributes.value("appid").toUInt();
		pMms->overtime = attributes.value("overtime").toInt();

	}
	else if (qName == "Ptp")
	{

		PPTPINFO pPtp = new PTPINFO();
		m_ied1->ptpList.push_back(pPtp);
		strcpy(pPtp->name,attributes.value("name").toAscii().data());
		strcpy(pPtp->desc,attributes.value("desc").toAscii().data());
		pPtp->ethIndex = attributes.value("ethIndex").toInt();
		pPtp->appid = attributes.value("appid").toUInt();


	}
	else if(qName == "Goose")
	{
		m_bGoose = true;

		if (m_bSubNet && m_bIed)
		{
			PGOOSEINFO pGooseInfo = new GOOSEINFO();
			m_gooseInfo = pGooseInfo;
			//添加goose控制块
			m_ied1->gsList.push_back(pGooseInfo);
			pGooseInfo->customId = attributes.value("customId").toUInt();
			pGooseInfo->ethIndex = attributes.value("ethIndex").toUInt();
			pGooseInfo->appid = attributes.value("appid").toUInt();
			strcpy(pGooseInfo->prefix,attributes.value("prefix").toAscii().data());
			strcpy(pGooseInfo->mac,attributes.value("mac").toAscii().data());
			pGooseInfo->confRev = attributes.value("confRev").toUInt();
			pGooseInfo->ethIndex = attributes.value("ethIndex").toUInt();
			strcpy(pGooseInfo->gcRef,attributes.value("gcRef").toAscii().data());
			strcpy(pGooseInfo->datSet,attributes.value("datSet").toAscii().data());
			strcpy(pGooseInfo->goId,attributes.value("goId").toAscii().data());
			pGooseInfo->datCount = attributes.value("datCount").toUInt();
			pGooseInfo->vlan = attributes.value("vLan").toUInt();
			pGooseInfo->enable = attributes.value("enable").toUInt();
			pGooseInfo->t0 =  attributes.value("t0").toInt();
			pGooseInfo->t1 =  attributes.value("t1").toInt();
			pGooseInfo->attribId = attributes.value("attribId").toUInt();


			strcpy(pGooseInfo->reportCtrl.name,attributes.value("rpt_name").toAscii().data());
			strcpy(pGooseInfo->reportCtrl.datSet,attributes.value("rpt_datSet").toAscii().data());
			strcpy(pGooseInfo->reportCtrl.rptID,attributes.value("rpt_rptID").toAscii().data());
			pGooseInfo->reportCtrl.max = attributes.value("max").toInt();

		}
	}
	else if(qName == "ChnInfo")
	{
		m_bChnInfo = true;
		if (m_bSubNet && m_bIed && m_bGoose)
		{
			PGSCHNINFO  pGsChnInfo = new GSCHNINFO();
			//添加通道信息
			m_gooseInfo->chnList.push_back(pGsChnInfo);
			pGsChnInfo->index = attributes.value("index").toUInt();
			strcpy(pGsChnInfo->chnName,attributes.value("chnName").toAscii().data());
			strcpy(pGsChnInfo->datSetName,attributes.value("datSetName").toAscii().data());
			pGsChnInfo->type = attributes.value("type").toUInt();
			pGsChnInfo->flag = attributes.value("flag").toUInt();
			pGsChnInfo->enable = attributes.value("enable").toUInt();
			pGsChnInfo->dataType = attributes.value("dataType").toInt();
			pGsChnInfo->dataSize = attributes.value("dataSize").toInt();
			strcpy(pGsChnInfo->dataTypeStr,attributes.value("dataTypeStr").toAscii().data());
			strcpy(pGsChnInfo->strVal,attributes.value("strVal").toAscii().data());


		}
		if (m_bSubNet && m_bIed && m_bSmv)
		{
			PSVCHNINFO pSvChnInfo = new SVCHNINFO();
			m_svInfo->chnList.push_back(pSvChnInfo);

			pSvChnInfo->index = attributes.value("index").toUInt();
			pSvChnInfo->remoteIndex = attributes.value("remoteIndex").toUInt();
			strcpy(pSvChnInfo->chnName,attributes.value("chnName").toAscii().data());
			strcpy(pSvChnInfo->datSetName,attributes.value("datSetName").toAscii().data());

			pSvChnInfo->type = attributes.value("type").toUInt();
			pSvChnInfo->flag = attributes.value("flag").toUInt();
			pSvChnInfo->priRated = attributes.value("priRated").toFloat();
			pSvChnInfo->secRated = attributes.value("secRated").toFloat();
			pSvChnInfo->priReferVal = attributes.value("priReferVal").toFloat();
			pSvChnInfo->referToSamp = attributes.value("referToSamp").toInt();
			pSvChnInfo->overLaod = attributes.value("overLaod").toUInt();
			pSvChnInfo->sampLimit = attributes.value("sampLimit").toFloat();
			pSvChnInfo->twoADDiff = attributes.value("twoADDiff").toFloat();
			pSvChnInfo->range = attributes.value("range").toFloat();
			pSvChnInfo->blm = attributes.value("blm").toFloat();
			pSvChnInfo->smpMin = attributes.value("smpMin").toFloat();
			pSvChnInfo->freq = attributes.value("freq").toInt();
			pSvChnInfo->enable = attributes.value("enable").toUInt();
			pSvChnInfo->precipValue = attributes.value("precipValue").toFloat();

			pSvChnInfo->dataType = attributes.value("dataType").toInt();
			pSvChnInfo->dataSize = attributes.value("dataSize").toInt();
			strcpy(pSvChnInfo->dataTypeStr,attributes.value("dataTypeStr").toAscii().data());
			strcpy(pSvChnInfo->strVal,attributes.value("strVal").toAscii().data());

			pSvChnInfo->maxVal  = attributes.value("maxVal").toFloat();
			pSvChnInfo->minVal  = attributes.value("minVal").toFloat();
			pSvChnInfo->stepVal  = attributes.value("stepVal").toFloat();
			strcpy(pSvChnInfo->unitStr,attributes.value("unitStr").toAscii().data());

		}
	}
	else if(qName == "Smv")
	{
		m_bSmv = true;

		if (m_bSubNet && m_bIed)
		{
			PSVINFO pSvInfo = new SVINFO();
			m_svInfo = pSvInfo;
			//添加smv控制块
			m_ied1->svList.push_back(pSvInfo);

			pSvInfo->customId = attributes.value("customId").toUInt();
			pSvInfo->ethIndex = attributes.value("ethIndex").toUInt();
			pSvInfo->appid = attributes.value("appid").toUInt();
			strcpy(pSvInfo->prefix,attributes.value("prefix").toAscii().data());
			strcpy(pSvInfo->mac,attributes.value("mac").toAscii().data());
			pSvInfo->confRev = attributes.value("confRev").toUInt();
			strcpy(pSvInfo->svId,attributes.value("svId").toAscii().data());
			strcpy(pSvInfo->datSet,attributes.value("datSet").toAscii().data());
			strcpy(pSvInfo->ldDevName,attributes.value("ldDevName").toAscii().data());
			strcpy(pSvInfo->lnName,attributes.value("lnName").toAscii().data());
			pSvInfo->datCount = attributes.value("datCount").toUInt();
			pSvInfo->datSetType = attributes.value("datSetType").toUInt();
			pSvInfo->convertType = attributes.value("convertType").toUInt();
			pSvInfo->vlan = attributes.value("vLan").toUInt();
			pSvInfo->enable = attributes.value("enable").toUInt();
			pSvInfo->attribId = attributes.value("attribId").toUInt();
		}
	}
	else if(qName == "CurInfo")
	{
		if (m_bSubNet && m_bIed && m_bSmv)
		{

			m_svInfo->curInfo.priRated = attributes.value("priRated").toFloat();
			m_svInfo->curInfo.secRated = attributes.value("secRated").toFloat();
			m_svInfo->curInfo.priReferVal = attributes.value("priReferVal").toFloat();
			m_svInfo->curInfo.referToSamp = attributes.value("referToSamp").toInt();
			m_svInfo->curInfo.overLaod = attributes.value("overLaod").toUInt();
			m_svInfo->curInfo.sampLimit = attributes.value("sampLimit").toFloat();
			m_svInfo->curInfo.twoADDiff = attributes.value("twoADDiff").toFloat();
			m_svInfo->curInfo.precipValue =  attributes.value("precipValue").toFloat();

		}

	}
	else if(qName == "VolInfo")
	{
		if (m_bSubNet && m_bIed && m_bSmv)
		{
			m_svInfo->volInfo.priRated = attributes.value("priRated").toFloat();
			m_svInfo->volInfo.secRated = attributes.value("secRated").toFloat();
			m_svInfo->volInfo.priReferVal = attributes.value("priReferVal").toFloat();
			m_svInfo->volInfo.referToSamp = attributes.value("referToSamp").toInt();
			m_svInfo->volInfo.overLaod = attributes.value("overLaod").toUInt();
			m_svInfo->volInfo.sampLimit = attributes.value("sampLimit").toFloat();
			m_svInfo->volInfo.twoADDiff = attributes.value("twoADDiff").toFloat();
			m_svInfo->volInfo.precipValue =  attributes.value("precipValue").toFloat();


		}
	}

}

bool ClientDSE_ScdManagerImpl::analystStartEmelent(const QString &namespaceURI, 
												   const QString &localName,
												   const QString &qName, 
												   const QXmlAttributes &attributes)
{
	if (m_fileFlag == FL_SCD)
	{
		startParseScdFile(namespaceURI,localName,qName,attributes);
	}
	else if (m_fileFlag == FL_XML)
	{
		startParseXmlFile(namespaceURI,localName,qName,attributes);
	}

	return true;

}


void   ClientDSE_ScdManagerImpl::endParseScdFile(const QString &namespaceURI, const QString &localName, const QString &qName)
{
	int qNameIndex = m_qNameMap.value(qName);

	switch(qNameIndex)
	{
	case 1:  //strSubstation
		{
			bSubstation = false;
		}
		break;
	case 2: //strCommunication
		{
			bCommunication = false;
		}
		break;
	case 3: //strSubNetwork
		{
			bWrite = false;
			bSubNetwork = false;
		}
		break;
	case 4: //strConnectedAP
		{
			bConnectedAP = false;
		}
		break;
	case 5:  //strGSE
		{

			bGSE = false;
		}
		break;
	case 6:  //strSMV
		{
			bSMV = false;
		}
		break;
	case 7: //strP
		{
			bP = false;
		}
		break;
	case 16://strMinTime
		{
			bMinTime = false;
		}
		break;
	case 17:  //strMaxTime
		{
			bMaxTime = false;
		}
		break;
	case 19:  //strIed
		{
			bIed = false;
		}
		break;
	case 20:  //strAccessPoint
		{
			bAccessPoint = false;
		}
		break;
	case 21:  //strService
		{
			bService = false;
		}
		break;
	case 22:  //strLDevice
		{
			bLdevice = false;
		}
		break;
	case 23:  //strLN0
		{
			bLn0 = false;
		}
		break;
	case 24:
		{
			bDataSet = false;
		}
		break;
	case 25:
		{
			bFcda = false;
		}
		break;
	case 33:  //Inputs
		{
			bInputs = false;
		}
		break;
	case 26:
		{
			// bGSEControl = false;
		}
		break;
	case 27:
		{
			bSampledValueControl = false;
		}
		break;
	case 31:
		{
			bLn = false;
		}
		break;
	case 32:
		{	
			bDoi = false;
		}
		break;
	}


	// if (qName == strSubstation)
	//{
	//bSubstation = false;

	//}
	//else if (qName == strCommunication)
	//{
	//bCommunication = false;
	//m_bComLayer = false;

	//}
	//else if (qName == strSubNetwork)
	//{
	// bWrite = false;
	// bSubNetwork = false;
	//}
	//else if (qName == strConnectedAP)
	//{

	// bConnectedAP = false;
	//}
	//else if (qName == strGSE)
	//{

	// bGSE = false;
	//}

	//else if (qName == strSMV)
	//{

	// bSMV = false;
	//}

	//else if (qName == strP)
	//{

	// bP = false;
	//}
	// else if (qName == strMinTime)
	//{
	// bMinTime = false;


	//}
	//else if (qName == strMaxTime)
	//{
	// bMaxTime = false;

	//}
	//else if  (qName == strIed)
	//{
	// bIed = false;


	//}
	//else if  (qName == strAccessPoint)
	//{
	// bAccessPoint = false;


	//}
	//else if  (qName == strService)
	//{
	// bService = false;


	//}
	//else if  (qName == strLDevice)
	//{
	// bLdevice = false;


	//}
	//else if  (qName == strLN0)
	//{
	// bLn0 = false;
	//


	//}
	//else if  (qName == strDataSet)
	//{
	// bDataSet = false;


	//}
	//else if  (qName == strFcda)
	//{
	// bFcda = false;


	//}
	//else if  (qName == strGSEControl)
	//{
	//// bGSEControl = false;


	//}
	//else if  (qName == strSampledValueControl)
	//{
	// bSampledValueControl = false;

	//}
	//else if (qName == strLN)
	//{

	// bLn = false;
	//}
	// else if (qName == strDoi)
	//{	
	// bDoi = false;
	//}
	// else if (qName == "SDI")
	//{

	// bSDI = false;
	// bMax = false;
	// bMin = false;
	// bUnit = false;
	// bStep = false;
	// strSDI = "";

	//}
	// else if (qName == "DAI")
	// {
	//strDAI = "";
	// }
	//else if (qName == "Val")
	//{	
	//bMax = false;	
	//bMin = false;
	//bUnit = false;
	//bStep = false;
	//}


}

void   ClientDSE_ScdManagerImpl::endParseXmlFile(const QString &namespaceURI, const QString &localName,
												 const QString &qName)
{

	if (qName == "PupInfo")
	{

	}
	else if(qName == "TimeSync")
	{

	}
	else if(qName == "NetCfg")
	{
		m_bNetCfg = false;
	}
	else if(qName == "Eth")
	{
		m_bEthCfg = false;
	}
	else if(qName == "Ip")
	{

	}
	else if(qName == "NetWork")
	{
		m_bNetWork = false;
	}
	else if(qName == "SubNet")
	{
		m_bSubNet = false;
	}
	else if(qName == "Ied")
	{
		m_bIed = false;
	}
	else if(qName == "Goose")
	{
		m_bGoose = false;
	}
	else if(qName == "ChnInfo")
	{
		m_bChnInfo = false;
	}
	else if(qName == "Smv")
	{
		m_bSmv = false;
	}
	else if(qName == "CurInfo")
	{

	}
	else if(qName == "VolInfo")
	{

	}


}

bool ClientDSE_ScdManagerImpl::analystEndEmelent(const QString &namespaceURI, 
												 const QString &localName,
												 const QString &qName)
{

	if (m_fileFlag == FL_SCD)
	{
		endParseScdFile(namespaceURI,localName,qName);
	}
	else if (m_fileFlag == FL_XML)
	{
		endParseXmlFile(namespaceURI,localName,qName);
	}


	return true;
}


PINTERNAL_IED1	ClientDSE_ScdManagerImpl::findIED(QString iedName)
{

	for (int i = 0 ;i < m_sta.iedList.size() ; i++)
	{

		PINTERNAL_IED1 pIed = m_sta.iedList[i];

		if (pIed->iedName == iedName)
		{
			return pIed;
		}

	}
	return NULL;

}

PSMVGSE ClientDSE_ScdManagerImpl::findSMVGse(QString name,quint8 ctrl,QString inst)
{
	if (!m_ied)
		return NULL;

	for (int i = 0 ; i< m_ied->smvGseList.count(); i++)
	{
		PSMVGSE msvgse = m_ied->smvGseList.at(i);

		if ( (msvgse->cbName == name) && (msvgse->inst ==  inst))
		{
			return msvgse;
		}
	}

	return NULL;

}

PDATASET ClientDSE_ScdManagerImpl::findDataSet(QString name)
{

	if (!m_ied)
		return NULL;

	for (int i = 0 ; i < m_dataSetList.count() ; i++)
	{
		PDATASET dataSet = m_dataSetList.at(i);

		if (dataSet->name == name)
		{
			//把有用的移除
			m_dataSetList.takeAt(i);
			return dataSet;
		}
	}

	return NULL;


}


void  ClientDSE_ScdManagerImpl::setFcdaLnType(QString prefix,QString lnClass,QString inst,QString lnType)
{

	QString ref = QString("%1$%2$%3$%4")
		.arg(prefix)
		.arg(lnClass)
		.arg(m_inst)
		.arg(inst);

	QList<PFCDA> list =m_lnTypeMap.values(ref);
	for (int i = 0 ;i < list.count() ; i++)
	{
		PFCDA pFcda= list.at(i);

		if (pFcda)
			pFcda->lnType = lnType;

	}




	return ;

	//不用  2014-5-8

	for (int i = 0 ;i < m_ied->smvGseList.count() ; i++)
	{
		PSMVGSE pSmvGse = m_ied->smvGseList[i];

		for (int j = 0 ;j < pSmvGse->dataSetList.count() ; j++)
		{
			PDATASET  pDataSet = pSmvGse->dataSetList[j];

			for (int l = 0 ; l < pDataSet->fcdaList.count() ; l++)
			{
				PFCDA pFcda = pDataSet->fcdaList[l];

				if  ( (pFcda->prefix == prefix) && (pFcda->lnClass == lnClass) && (pFcda->lnInst == inst) && (pFcda->ldInst == m_inst )  )
				{
					pFcda->lnType = lnType;

				}

			}
		}
	}
}

PLNODE_TYPE ClientDSE_ScdManagerImpl::findLnType(QString id,QString lnClass)
{
	/*for (int i = 0 ; i < m_sta.lnTypeList.size() ; i++)
	{
	PLNODE_TYPE pLN = m_sta.lnTypeList[i];

	if ( (QString("%1").arg(pLN->id) == id ) && ( QString("%1").arg(pLN->lnClass) == lnClass) )
	{
	return pLN;
	}

	}


	return NULL;*/

	PLNODE_TYPE pLnType = m_sta.lnTypeListMap.value(id);

	return pLnType;

}


//bool    ClientDSE_ScdManagerImpl::findDa(QString id)
//{

// for (int i = 0 ; i < m_sta.doTypeList.count() ; i++)
// {

//	 PDO_TYPE pDoType  = m_sta.doTypeList[i];

//	 for (int j = 0 ; j < pDoType->daList.count() ; j++)
//	 {
//		 PDA pDaType = pDoType->daList[j];

//		 if (QString("%1").arg(pDaType->type) == id)
//		 {
//			return true;

//		 }
//	 }

//	for (int j = 0 ; j < pDoType->sdoList.count() ; j++)
//	{
//		PSDO pSdoType = pDoType->sdoList[j];
//		 if (QString("%1").arg(pSdoType->type) == id)
//		 {
//			return true;

//		 }
//	}
// }

// return false;
// 


//}

//bool   ClientDSE_ScdManagerImpl::findDo(QString id)
//{

// for (int i = 0 ; i< m_sta.lnTypeList.count() ;i++)
// {

//	 PLNODE_TYPE pLn = m_sta.lnTypeList[i];

//	 for (int j = 0 ; j < pLn->doList.count() ; j++)
//	 {

//		 PDO pDo = pLn->doList[j];

//		 if (QString("%1").arg(pDo->type) == id )
//			 return true;
//	 }
// }

// return false;

//}


PFCDA  ClientDSE_ScdManagerImpl::findFcda(QString prefix,QString lnClass,QString inst,QString doName,QString daName)
{

	if (!m_ied)
		return NULL;

	for (int i = 0 ; i< m_ied->smvGseList.count(); i++)
	{
		PSMVGSE msvgse = m_ied->smvGseList.at(i);

		if (msvgse->control == GSMV_TYPE_GOOSE)
		{

			for (int j= 0 ; j < msvgse->dataSetList.count() ; j++)
			{

				PDATASET  pDataSet = msvgse->dataSetList[j];

				for (int l = 0 ; l < pDataSet->fcdaList.count() ; l++)
				{

					PFCDA  pFcda = pDataSet->fcdaList[l];

					if (daName == "")
					{
						if ((pFcda->prefix == prefix) && (pFcda->lnClass == lnClass) && (pFcda->lnInst == inst) && (pFcda->doName == doName))
						{
							return pFcda;
						}
					}
					else
					{
						if ((pFcda->prefix == prefix) && (pFcda->lnClass == lnClass) && (pFcda->lnInst == inst) && (pFcda->doName == doName) && (pFcda->daName == daName))
						{
							return pFcda;
						}
					}
				}

			}
		}
	}


	return NULL;


}


PDA ClientDSE_ScdManagerImpl::findDaType(QString name,QString bType,QString type,QString fc)
{
	if (!m_pDoType)
		return NULL;

	for (int i = 0 ; i < m_pDoType->daList.count() ; i++)
	{

		PDA pDa = m_pDoType->daList[i];

		if ((QString("%1").arg(pDa->name) == name )  && (QString("%1").arg(pDa->bType) == bType)  && (QString("%1").arg(pDa->type) == type ) && (QString("%1").arg(pDa->fc) == fc ))
		{
			return pDa;
		}

	}

	return NULL;

}

//bool  ClientDSE_ScdManagerImpl::getDoCdc(QString id,QString &cdc)
//{

// for (int m= 0 ; m < m_sta.doTypeList.count() ;m++)
// {
//		 PDO_TYPE pDoType = m_sta.doTypeList[m];

//		 //<DOType id="GDNR_CN_INC_Mod" cdc="INC">
//		 if ( id == QString("%1").arg(pDoType->id) )//
//		 {
//			cdc = QString(pDoType->cdc);
//			return true;

//		 }
// }

// return false;

//}

//bool  ClientDSE_ScdManagerImpl:: getDoType(QString sdoType,QString daName,QString &retType,QString &retbType)
//{

// 	 for (int m= 0 ; m < m_sta.doTypeList.count() ;m++)
//	 {
//			 PDO_TYPE pDoType = m_sta.doTypeList[m];

//			 //<DOType id="GDNR_CN_INC_Mod" cdc="INC">
//			 if ( sdoType == QString("%1").arg(pDoType->id) )//
//			 {
//				 for (int j = 0 ; j < pDoType->daList.count() ; j++)
//				 {
//					 PDA pDaType = pDoType->daList[j];

//					 if (QString(pDaType->name) == daName)
//					 {
//						
//						
//						 retType = QString("%1").arg(pDaType->type);
//						 retbType = QString("%1").arg(pDaType->bType);

//						 return true;
//					 }
//				 }
//			 }


//	 }

//     return false;

//}

bool  ClientDSE_ScdManagerImpl:: getBDAType(QString daType,QString bdaName,QString &retBdaType)
{

	PDA_TYPE pDaType = m_sta.daTypeListMap.value(daType);
	if (pDaType)
	{
		PBDA_TYPE pBdaType = pDaType->bdaTypeListMap.value(bdaName);
		if (pBdaType)
		{
			if (QString("%1").arg(pBdaType->type) !="" )  //type不是空就说是结构还需要往下解析BDATYPE
			{
				//以后再加 ???
			}
			else
			{
				retBdaType = QString("%1").arg(pBdaType->bType);
				return true;
			}
		}

	}

	return false;

	/* for (int i = 0 ;i < m_sta.daTypeList.count() ;i++)
	{
	PDA_TYPE pDaType = m_sta.daTypeList[i];

	if ( QString("%1").arg(pDaType->id) == daType)
	{

	for (int j= 0; j < pDaType->bdaTypeList.count() ; j++)
	{
	PBDA_TYPE pBdaType = pDaType->bdaTypeList[j];

	if (QString("%1").arg(pBdaType->name) ==  bdaName)
	{
	if (QString(pBdaType->type) == "")
	retBdaType =  QString("%1").arg(pBdaType->bType);
	else
	retBdaType =  QString("%1").arg(pBdaType->type);

	return true;
	}
	}
	}

	}


	return false;*/

}

//bool  ClientDSE_ScdManagerImpl::getBDARef(QString daType,QString &bdaRef,QString &retBdaType,QString &retType)
//{

// for (int i = 0 ;i < m_sta.daTypeList.count() ;i++)
// {
//	 PDA_TYPE pDaType = m_sta.daTypeList[i];

//	 if ( QString("%1").arg(pDaType->id) == daType)
//	 {
//		 for (int j= 0; j < pDaType->bdaTypeList.count() ; j++)
//		 {
//			 PBDA_TYPE pBdaType = pDaType->bdaTypeList[j];

//			 bdaRef = QString(pBdaType->name);
//			 retBdaType = QString(pBdaType->bType);
//			 retType = QString(pBdaType->type);

//			 return true;
//		 }
//	 }
// }


// return false;
//}

t_uint8  ClientDSE_ScdManagerImpl::getDataSizeValue(t_uint8 dataType)
{
	t_uint8 dataSize = 0;
	switch(dataType)
	{
	case _GSDATA_TYPE::GV_NONE:
		dataSize = 0;
		break;
	case _GSDATA_TYPE::GVE_INT8U:
	case  _GSDATA_TYPE::GV_Dbpos:
	case _GSDATA_TYPE::GV_Sbpos:
	case _GSDATA_TYPE::GVE_BOOLEAN:
		dataSize = 1;
		break;
	case _GSDATA_TYPE::GVE_INT32:
	case _GSDATA_TYPE::GVE_INT32U:
	case _GSDATA_TYPE::GVE_FLOAT32:
		dataSize = 4;
		break;
	case _GSDATA_TYPE::GVE_Octet64:
		dataSize = 64;
		break;
	case _GSDATA_TYPE::GVE_Unicode255:
		dataSize = 255;
		break;
	case _GSDATA_TYPE::GVE_VisString255:
		dataSize = 255;
		break;
	case _GSDATA_TYPE::GVE_Timestamp:
		dataSize = 12;
		break;
	case _GSDATA_TYPE::GV_Quality:
		dataSize = 4;
		break;

	}

	return dataSize;

}

t_uint8   ClientDSE_ScdManagerImpl::getDataTypeValue(QString typeString,t_char *strDefVal)
{

	if (typeString == "BOOLEAN")
	{
		sprintf(strDefVal,"0");
		return _GSDATA_TYPE::GVE_BOOLEAN;
	}
	else if (typeString == "INT8U")
	{
		sprintf(strDefVal,"0");
		return _GSDATA_TYPE::GVE_INT8U;
	}
	else if (typeString == "INT8")
	{
		sprintf(strDefVal,"0");
		return _GSDATA_TYPE::GV_INT8;
	}
	else if (typeString == "INT32")
	{
		sprintf(strDefVal,"0");
		return _GSDATA_TYPE::GVE_INT32;

	}
	else if (typeString == "INT32U")
	{
		sprintf(strDefVal,"0");
		return _GSDATA_TYPE::GVE_INT32U;

	}
	else if (typeString == "Timestamp")
	{
		return _GSDATA_TYPE::GVE_Timestamp;

	}
	else if (typeString == "Check")
	{
		return _GSDATA_TYPE::GVE_Check;

	}
	else if (typeString == "FLOAT32")
	{
		sprintf(strDefVal,"0.00");
		return _GSDATA_TYPE::GVE_FLOAT32;

	}
	else if (typeString == "Struct")
	{
		return _GSDATA_TYPE::GVE_Struct;

	}
	else if (typeString == "Enum")
	{
		return _GSDATA_TYPE::GVE_Enum;

	}
	else if (typeString == "Octet64")
	{
		return _GSDATA_TYPE::GVE_Octet64;

	}
	else if (typeString == "Unicode255")
	{
		sprintf(strDefVal,"");
		return _GSDATA_TYPE::GVE_Unicode255;

	}
	else if (typeString == "VisString255")
	{
		sprintf(strDefVal,"");
		return _GSDATA_TYPE::GVE_VisString255;

	}
	else if (typeString == "Dbpos")
	{
		sprintf(strDefVal,"1");
		return _GSDATA_TYPE::GV_Dbpos;

	}
	else if (typeString == "Quality")
	{
		return _GSDATA_TYPE::GV_Quality;

	}
	else if (typeString == "SPS")
	{
		sprintf(strDefVal,"0");
		return _GSDATA_TYPE::GV_Sbpos;
	}


	t_uint8 bRet = _GSDATA_TYPE::GV_NONE;
	return bRet;

}


QString  ClientDSE_ScdManagerImpl::getDaNameFromCdc(QString strCdc)
{
	if (strCdc =="ASG")  //模拟定值
	{
		return "setMag";
	}
	else  if (strCdc =="SPG")  //单点定值
	{
		return "setVal";
	}
	else if (strCdc == "DPS") //双点状态
	{
		return "stVal";
	}
	else if (strCdc == "ING")  //整数状态定值
	{
		return "setVal";
	}
	else if (strCdc == "ACT") //7.3.5  保护激活信息
	{
		return "general";
	}
	else if (strCdc == "SPC") //  7.5.2  可控的单点
	{
		return "stVal";
	}
	else if (strCdc == "SPS") //  7.3.2  单点状态信息  
	{
		return "stVal";
	} else if (strCdc == "CMV")
	{
		return "cVal";
	}
	else if (strCdc == "ACD")
	{
		return "general";
	}
	else     
	{
		QString str = "";

	}  

	return "";
}

QString  ClientDSE_ScdManagerImpl::getDataType(QString lnType,QString lnClass, QString inst,QString doName,QString fc,QString &daName,bool &bRet)
{
	PLNODE_TYPE pLn = m_sta.lnTypeListMap.value(lnType);
	if (pLn == NULL)
		return "";

	if ((QString("%1").arg(pLn->id) == lnType) && (QString("%1").arg(pLn->lnClass) == lnClass))
	{
		PDO pDo = pLn->doTypeListMap.value(doName);
		if (pDo)
		{
			PDO_TYPE pDoType = NULL ;
			pDoType  = m_sta.doTypeListMap.value(QString("%1").arg(pDo->type));

			//<DOType id="GDNR_CN_INC_Mod" cdc="INC">
			if (pDoType)
			{
				//先从常用列表中查找到没有在循环找，这样速度也快些
				QString strDataType = m_cdcMap.value(QString("%1/%2").arg(pDoType->cdc).arg(daName));
				if (strDataType != "")
					return strDataType;

				for (int l = 0 ; l< pDoType->daList.count() ;l++)
				{

					PDA pDaType = pDoType->daList[l];

					QStringList   strList = daName.split(".");

					QString tmpDaName = daName;
					QString tmpBdaName = daName;

					if (strList.count() >=2)
					{
						tmpDaName = strList.at(0);
						tmpBdaName = strList.at(1);
					}

					//找到DA
					if ( (tmpDaName == "") || ((QString("%1").arg(pDaType->name) == tmpDaName) && (QString("%1").arg(pDaType->fc) == fc)) )
					{
						if (QString("%1").arg(pDaType->type) !="" )  //type不是空就说是结构还需要往下解析DATYPE
						{

							if ( (QString("%1").arg(pDaType->bType) == "Dbpos") || (QString("%1").arg(pDaType->type) == "Dbpos"))
							{
								return "Dbpos";
							}
							else
							{
								QString retBType = "";
								getBDAType(QString("%1").arg(pDaType->type),tmpBdaName,retBType);
								if ( QString("%1").arg(pDaType->bType) == "Struct" )
									bRet = true;

								return retBType;
							}										
						}
						else
						{
							return QString("%1").arg(pDaType->bType);

						}
					}

				}
			}
		}
	}

	return "";

	//QString tmpDoName = doName;
	//QString tmpSdoName="";
	//QString tmpNewDaName = "";  //daName为空，新生成
	//QString tmpNewDaNameRef = "";

	//QStringList strDOList = doName.split(".");  //doName="PhV.phsA"
	//if (strDOList.count() >1)
	//{
	// tmpDoName = strDOList.at(0);        //PhV
	// tmpSdoName = strDOList.at(1);       //phsA
	//}

	// QStringList   strList = daName.split(".");

	//QString tmpDaName   = daName;
	//QString tmpBdaName  = "";
	//QString tmpBdaName1  = "";

	// if (strList.count() >=3)   
	// {
	// tmpDaName = strList.at(0);
	// tmpBdaName = strList.at(1);
	// tmpBdaName1 = strList.at(2);
	// }
	// else if (strList.count() == 2)
	// {
	//   tmpDaName = strList.at(0);
	//   tmpBdaName = strList.at(1);
	// }


	//for (int i = 0 ; i < m_sta.lnTypeList.count() ; i++)
	//{
	// PLNODE_TYPE pLn = m_sta.lnTypeList[i];

	// bool bFind = false;

	// if (lnClass == "LLN0")   //如果是LLN0 就没有lnType
	// {
	//	 if (QString("%1").arg(pLn->lnClass) == lnClass)
	//	 {
	//		bFind = true;

	//	 }
	// }
	// else
	// {
	//	 if ((QString("%1").arg(pLn->id) == lnType) && (QString("%1").arg(pLn->lnClass) == lnClass))  //找到 <LNodeType lnClass="GGIO" id="SAC_IED_GGIO_DI" desc="遥信">
	//	 {
	//		bFind = true;
	//	 }	

	// }



	// if (bFind) 
	// {

	//	 for (int j = 0 ; j < pLn->doList.count() ; j++)
	//	 {

	//		 PDO pDo = pLn->doList[j];

	//		 if (QString("%1").arg(pDo->name) == tmpDoName) //找到<DO type="CN_SPS" name="Alm1" desc="告警1" transient="false"></DO>
	//		 {
	//			 for (int m= 0 ; m < m_sta.doTypeList.count() ;m++)
	//			 {
	//				 PDO_TYPE pDoType = m_sta.doTypeList[m];

	//				 //<DOType id="GDNR_CN_INC_Mod" cdc="INC">
	//				 if ( QString("%1").arg(pDo->type) == QString("%1").arg(pDoType->id) )//
	//				 {
	//					 //if (QString(pDoType->cdc) == "SPS")
	//					 //{
	//						////return "SPS";

	//					 //}
	//					 //else if (QString(pDoType->cdc) == "SPS")
	//					 //{
	//						////return "Dbpos";
	//					 //}
	//					 //else
	//					 {		
	//						  PSDO  tmpPsdo  =NULL;
	//						  bool bFindPsdo = false;
	//						 if (daName=="")   //daName为空，则没有数据属性  根据cdc找到公共类型
	//						 {
	//					
	//							 if (tmpSdoName !="")
	//							 {
	//								  for (int l = 0; l < pDoType->sdoList.count(); l++)
	//								 {
	//									 PSDO pSdo = pDoType->sdoList[l];

	//									 if (QString("%1").arg(pSdo->name) == tmpSdoName)
	//									 {
	//										tmpPsdo = pSdo;
	//										bFindPsdo = true;
	//										break;

	//									 }
	//								 }
	//							 }
	//							
	//							 if (tmpPsdo)
	//							 { 
	//							
	//								 QString tmpCdc = "";
	//								 getDoCdc(tmpPsdo->type,tmpCdc);
	//								 sprintf(pDoType->cdc,"%s",tmpCdc.toAscii().data());

	//							 }
	//							 tmpNewDaName = getDaNameFromCdc(QString(pDoType->cdc) );
	//							 tmpDaName = tmpNewDaName;
	//							 strList = tmpNewDaName.split(".");
	//							 tmpNewDaNameRef = tmpNewDaName;

	//							 if (tmpNewDaName == "") //没找到公共类型
	//							    return "";	

	//							 QString retType="",retBtype = "";

	//							 //先看下DO下面的这个da是不是直接就是基本类型
	//							 bool bRet11 =  getDoType(QString("%1").arg(pDoType->id),tmpNewDaName,retType,retBtype);

	//							  if ( (retType=="") && (bRet11) )
	//							  {
	//								 
	//								  daName = tmpNewDaNameRef;
	//								  return retBtype;
	//							  }
	//						 }
	//						

	//						
	//						 PDA   tmpPda   = NULL;
	//						 if(!bFindPsdo)
	//						 {
	//							 for (int l = 0; l < pDoType->sdoList.count(); l++)
	//							 {
	//								 PSDO pSdo = pDoType->sdoList[l];

	//								 if (QString("%1").arg(pSdo->name) == tmpSdoName)
	//								 {
	//									tmpPsdo = pSdo;
	//									break;

	//								 }
	//							 }
	//						 }

	//						 if (tmpPsdo == NULL)
	//						 {
	//							 for (int l = 0 ; l< pDoType->daList.count() ;l++)
	//							 {
	//								 PDA pDaType = pDoType->daList[l];

	//								 if ((QString("%1").arg(pDaType->name) == tmpDaName) && (QString("%1").arg(pDaType->fc) == fc))
	//								 {
	//									tmpPda = pDaType;

	//									if (strList.count() == 1)
	//									{
	//										if (QString(pDaType->type) =="")
	//										{
	//										   return QString("%1").arg(pDaType->bType);
	//										} 
	//									}

	//									break;
	//								 }

	//							 }
	//						 }

	//						  if (strList.count() >=3)
	//						  {
	//							  QString retType="",retBtype = "";

	//							 if (tmpPsdo)
	//							 {
	//								
	//								 getDoType(tmpPsdo->type,tmpDaName,retType,retBtype);
	//								 getBDAType(retType,tmpBdaName,retType);
	//							     getBDAType(retType,tmpBdaName1,retBtype);

	//							 }
	//							 else if (tmpPda)
	//							 {
	//								 getBDAType(tmpPda->type,tmpBdaName,retType);
	//							     getBDAType(retType,tmpBdaName1,retBtype);
	//							 }
	//							
	//							 return retBtype;

	//						  }
	//						  else if (strList.count() ==2)
	//						  {
	//							   QString retType="",retBtype = "";
	//							   if (tmpPsdo)
	//							   {
	//								  QString tmpDaType = "";
	//								  getDoType(tmpPsdo->type,tmpDaName,retType,retBtype);
	//							      getBDAType(retType,tmpBdaName,retBtype);
	//							   }
	//							   else if (tmpPda)
	//							   {
	//								   getBDAType(tmpPda->type,tmpBdaName,retBtype);
	//							   }
	//							
	//							  return retBtype;
	//						  }	
	//						  else if (strList.count() ==1)
	//						  {
	//							    QString retType="",retBtype = "";
	//							   if (tmpPsdo)
	//							   {	
	//								   getDoType(tmpPsdo->type,tmpDaName,retType,retBtype);

	//								   if (retType!="")  //type不等于空，还有一层节点
	//								   {
	//									    QString strBdaRef = "";
	//										getBDARef(retType,strBdaRef,retBtype,retType);  //生成新的 daName

	//										if (retType != "")
	//										{
	//											tmpNewDaNameRef = QString("%1.%2").arg(tmpNewDaNameRef).arg(strBdaRef);
	//											getBDARef(retType,strBdaRef,retBtype,retType);  //生成新的 daName
	//											tmpNewDaNameRef = QString("%1.%2").arg(tmpNewDaNameRef).arg(strBdaRef);
	//											daName = tmpNewDaNameRef;
	//											return retBtype;

	//										}
	//										else
	//										{
	//											tmpNewDaNameRef = QString("%1.%2").arg(tmpNewDaNameRef).arg(strBdaRef);
	//											daName = tmpNewDaNameRef;
	//											return retBtype;

	//										}
	//								   }
	//							   }
	//							   else if (tmpPda)
	//							   {
	//									 QString strBdaRef = "";
	//									 getBDARef(tmpPda->type,strBdaRef,retBtype,retType);  //生成新的 daName

	//									 tmpNewDaNameRef = QString("%1.%2").arg(tmpNewDaNameRef).arg(strBdaRef);

	//									 daName = tmpNewDaNameRef;
	//							   }

	//							   return retBtype;
	//						  }
	//					 }
	//	
	//				 }
	//			 }
	//		 }
	//	 }

	//}

	//}

	//return "";
}




bool   ClientDSE_ScdManagerImpl::getDataType(PFCDA fcda,QList<PCUSTOM_DA> &daList)
{

	QString doName = fcda->doName;
	QString daName = fcda->daName;
	QString lnClass = fcda->lnClass;
	QString lnType  = fcda->lnType;

	QString tmpDoName = doName;
	QString tmpSdoName="";

	QStringList strDOList = doName.split(".");  //doName="PhV.phsA"
	if (strDOList.count() >1)
	{
		tmpDoName  = strDOList.at(0);        //PhV
		tmpSdoName = strDOList.at(1);        //phsA
	}


	//QMapIterator<QString, PLNODE_TYPE> lnTypeListIterator(m_sta.lnTypeListMap);
	//while (lnTypeListIterator.hasNext()) 
	//{

	//lnTypeListIterator.next();

	//PLNODE_TYPE pLn = lnTypeListIterator.value();
	PLNODE_TYPE pLn = m_sta.lnTypeListMap.value(lnType);

	bool bFind = false;

	//if (lnClass == "LLN0")   //如果是LLN0 就没有lnType
	//{
	// if (QString("%1").arg(pLn->lnClass) == lnClass)
	// {
	//	bFind = true;

	// }
	//}
	//else
	//{
	//if ((QString("%1").arg(pLn->id) == lnType) && (QString("%1").arg(pLn->lnClass) == lnClass))  //找到 <LNodeType lnClass="GGIO" id="SAC_IED_GGIO_DI" desc="遥信">
	//if (QString("%1").arg(pLn->id) == lnType)   //2014-4-17 去掉比较lnClass pLn->id应该是唯一的
	//{
	//	bFind = true;
	//}	

	//}

	//if (bFind == false)  //没找到继续查找
	//	 continue;
	if (pLn == NULL)
		return false;

	PDO pDo = pLn->doTypeListMap.value(tmpDoName);

	if (pDo)
	{
		PDO_TYPE pDoType = m_sta.doTypeListMap.value(QString("%1").arg(pDo->type));
		if (tmpSdoName != "")
		{
			PSDO pSdo = pDoType->sdoListMap.value(tmpSdoName);
			PDO_TYPE pDoType1 = m_sta.doTypeListMap.value(QString("%1").arg(pSdo->type));
			if (pDoType1)
				return getDaType(fcda,daList,pDoType1);  //获得DA的类型
		}
		else   //查找DO下的DA节点
		{
			return getDaType(fcda,daList,pDoType);  //获得DA的类型
		}
	}

	// }

	return false;

	//QString doName = fcda->doName;
	//QString daName = fcda->daName;
	//QString lnClass = fcda->lnClass;
	//QString lnType  = fcda->lnType;

	//QString tmpDoName = doName;
	//QString tmpSdoName="";

	//QStringList strDOList = doName.split(".");  //doName="PhV.phsA"
	//if (strDOList.count() >1)
	//{
	// tmpDoName  = strDOList.at(0);        //PhV
	// tmpSdoName = strDOList.at(1);        //phsA
	//}



	//for (int i = 0 ; i < m_sta.lnTypeList.count() ; i++)
	//{
	// PLNODE_TYPE pLn = m_sta.lnTypeList[i];

	// bool bFind = false;

	// if (lnClass == "LLN0")   //如果是LLN0 就没有lnType
	// {
	//	 if (QString("%1").arg(pLn->lnClass) == lnClass)
	//	 {
	//		bFind = true;

	//	 }
	// }
	// else
	// {
	//	 if ((QString("%1").arg(pLn->id) == lnType) && (QString("%1").arg(pLn->lnClass) == lnClass))  //找到 <LNodeType lnClass="GGIO" id="SAC_IED_GGIO_DI" desc="遥信">
	//	 {
	//		bFind = true;
	//	 }	

	// }

	// if (bFind == false)  //没找到继续查找
	//	 continue;


	//	 for (int j = 0 ; j < pLn->doList.count() ; j++)   //LNodeType类型下的DO列表
	// {
	//	 PDO pDo = pLn->doList[j];

	//	 if (QString("%1").arg(pDo->name) == tmpDoName) //找到<DO type="CN_SPS" name="Alm1" desc="告警1" transient="false"></DO>
	//	 {
	//		  for (int m= 0 ; m < m_sta.doTypeList.count() ;m++)
	//		 {
	//				 PDO_TYPE pDoType = m_sta.doTypeList[m];

	//				 //<DOType id="GDNR_CN_INC_Mod" cdc="INC">
	//				 if ( QString("%1").arg(pDo->type) == QString("%1").arg(pDoType->id) )//查到DO节点
	//				 {
	//					 
	//					 if (tmpSdoName != "")  //查找DO下的SDO节点
	//					 {
	//						for (int dolistIndex = 0 ; dolistIndex < pDoType->sdoList.count() ; dolistIndex++)
	//						{
	//							PSDO pSdo = pDoType->sdoList.at(dolistIndex);

	//							if (QString(pSdo->name) == tmpSdoName)  //查到sdo节点
	//							{

	//								  for (int doIndex= 0 ; doIndex < m_sta.doTypeList.count() ;doIndex++)  //再从do列表中查找对应的sdo节点
	//								  {
	//									   PDO_TYPE pDoType1 = m_sta.doTypeList[doIndex];
	//									   if ( QString("%1").arg(pSdo->type) == QString("%1").arg(pDoType1->id) )//查到DO节点
	//									   {
	//											return getDaType(fcda,daList,pDoType1);  //获得DA的类型

	//									   }
	//								  }
	//							}

	//						}
	//					 }
	//					 else   //查找DO下的DA节点
	//					 {
	//						return getDaType(fcda,daList,pDoType);  //获得DA的类型

	//					 }
	//				 }
	//		  }
	//	 }

	// }

	// }

	//return false;
}

bool ClientDSE_ScdManagerImpl::getDaType(PFCDA fcda,QList<PCUSTOM_DA> &daList,PDO_TYPE pDoType)
{
	QString daName = fcda->daName;
	QString fc     = fcda->fc;

	QString tmpNewDaName = "";  //daName为空，新生成
	QString tmpNewDaNameRef = "";
	QString tmpDaName    = daName;
	QString tmpBdaName   = "";
	QString tmpBdaName1  = "";
	bool    bDaEmpty = false;
	int     daCount = 0;
	int     tmpDaIndex = 0;

	if (daName =="")
	{
		bDaEmpty = true; 
	}
	else   
	{
		QStringList   strList = daName.split(".");
		if (strList.count() >=3)//cVal.mag.f
		{
			tmpDaName = strList.at(0);
			tmpBdaName = strList.at(1);
			tmpBdaName1 = strList.at(2);
			daCount = 3;
		}
		else if (strList.count() == 2)
		{
			tmpDaName  = strList.at(0);
			tmpBdaName = strList.at(1);
			daCount = 2;
		}
		else
		{
			daCount = 1;
		}	

	}


	for (int daIndex = 0 ; daIndex < pDoType->daList.count() ; daIndex++)  //DO下的DA列表
	{
		PDA  pDa= pDoType->daList.at(daIndex);  

		if (bDaEmpty)  //是空的，制造
		{
			if ( (QString(pDa->fc) == fc) && (QString(pDa->name) != "q") && (QString(pDa->name) != "t") )
			{
				PCUSTOM_DA pCustDa = new CUSTOM_DA();
				daList.append(pCustDa);

				pCustDa->daName = QString(pDa->name);

				if (QString(pDa->type) == "")  
				{
					pCustDa->daDataType = QString(pDa->bType);

				}
				else   //还没有到基本节点
				{
					if (strcmp(pDa->bType,"Enum") == 0)  //2014-04-09 如果是枚举类型就不用向下解析了
					{
						pCustDa->daDataType = "Enum";
					}
					else
					{
						makeDaName(pCustDa,QString(pDa->type));
					}
				}


			}


		}
		else
		{

			if (tmpDaName != QString(pDa->name))   
			{
				continue;	
			}

			if (daCount == 1) // cVal
			{
				PCUSTOM_DA pCustDa = new CUSTOM_DA();
				daList.append(pCustDa);

				pCustDa->daName =daName;
				pCustDa->daDataType = QString(pDa->bType);

				return true;
			}

			tmpDaIndex++;	

			bool bFind = false;
			QString typeStr = QString(pDa->type);
			PBDA_TYPE pBdaType1 = NULL;

			PDA_TYPE pDaType = m_sta.daTypeListMap.value(typeStr);

			// for (int tmpIndex = 0 ; tmpIndex < m_sta.daTypeList.count() ; tmpIndex++)
			//{
			//	 PDA_TYPE pDaType = m_sta.daTypeList.at(tmpIndex);

			//	 if (QString(pDaType->id) != typeStr )
			//		 continue;

			if (pDaType)
			{
				for (int bdaIndex = 0 ;bdaIndex < daCount-1;bdaIndex++)
				{
					if (pDaType == NULL)
						continue;

					switch(tmpDaIndex)
					{
					case 1:
						{								
							PBDA_TYPE pBdaType = pDaType->bdaTypeListMap.value(tmpBdaName);// .mag
							if (pBdaType)
							{
								tmpDaIndex++;
								bFind = true;
								pBdaType1 = pBdaType;
								typeStr = QString(pBdaType->type);
								pDaType = m_sta.daTypeListMap.value(typeStr);
								break;
							}

						}
						break;
					case 2:
						{
							PBDA_TYPE pBdaType = pDaType->bdaTypeListMap.value(tmpBdaName1); //.f
							if (pBdaType)
							{
								tmpDaIndex++;
								bFind = true;
								pBdaType1 = pBdaType;
								typeStr = QString(pBdaType->type);
								break;
							}
						}
						break;
					}
				}

				if (tmpDaIndex >= daCount-1)    //到最基本的节点
				{
					PCUSTOM_DA pCustDa = new CUSTOM_DA();
					daList.append(pCustDa);

					pCustDa->daName =daName;
					if (pBdaType1)
						pCustDa->daDataType = QString(pBdaType1->bType);
					return true;
				}
				else
				{
					bFind = false; 
				}

			}

		}
	}
	return true;

	//QString daName = fcda->daName;
	//QString fc     = fcda->fc;

	//QString tmpNewDaName = "";  //daName为空，新生成
	//QString tmpNewDaNameRef = "";
	//QString tmpDaName    = daName;
	//QString tmpBdaName   = "";
	//QString tmpBdaName1  = "";
	//bool    bDaEmpty = false;
	//int     daCount = 0;
	//int     tmpDaIndex = 0;

	//if (daName =="")
	//{
	//bDaEmpty = true; 
	//}
	//else   
	//{
	// QStringList   strList = daName.split(".");
	// if (strList.count() >=3)//cVal.mag.f
	// {
	//	 tmpDaName = strList.at(0);
	//	 tmpBdaName = strList.at(1);
	//	 tmpBdaName1 = strList.at(2);
	//	 daCount = 3;
	// }
	// else if (strList.count() == 2)
	// {
	//	 tmpDaName  = strList.at(0);
	//	 tmpBdaName = strList.at(1);
	//	 daCount = 2;
	// }
	// else
	// {
	//	daCount = 1;
	// }	

	//}


	//for (int daIndex = 0 ; daIndex < pDoType->daList.count() ; daIndex++)  //DO下的DA列表
	//{
	// PDA  pDa= pDoType->daList.at(daIndex);  

	// if (bDaEmpty)  //是空的，制造
	// {
	//	 if ( (QString(pDa->fc) == fc) && (QString(pDa->name) != "q") && (QString(pDa->name) != "t") )
	//	 {
	//		 PCUSTOM_DA pCustDa = new CUSTOM_DA();
	//		 daList.append(pCustDa);

	//		 pCustDa->daName = QString(pDa->name);

	//		 if (QString(pDa->type) == "")  
	//		 {
	//			pCustDa->daDataType = QString(pDa->bType);
	//		
	//		 }
	//		 else   //还没有到基本节点
	//		 {
	//			 makeDaName(pCustDa,QString(pDa->type));
	//		 }


	//	 }


	// }
	// else
	// {

	//	if (tmpDaName != QString(pDa->name))   
	//	{
	//	   continue;	
	//	}

	//	if (daCount == 1) // cVal
	//	{
	//		PCUSTOM_DA pCustDa = new CUSTOM_DA();
	//		daList.append(pCustDa);

	//		pCustDa->daName =daName;
	//		pCustDa->daDataType = QString(pDa->bType);

	//		return true;
	//	}

	//	tmpDaIndex++;	
	//	
	//	 bool bFind = false;
	//	 QString typeStr = QString(pDa->type);
	//	 PBDA_TYPE pBdaType1 = NULL;
	//	 for (int tmpIndex = 0 ; tmpIndex < m_sta.daTypeList.count() ; tmpIndex++)
	//	 {
	//		 PDA_TYPE pDaType = m_sta.daTypeList.at(tmpIndex);

	//		 if (QString(pDaType->id) != typeStr )
	//			 continue;

	//		 switch(tmpDaIndex)
	//		 {
	//		 case 1:
	//			 {
	//				 for (int bdaIndex = 0 ; bdaIndex < pDaType->bdaTypeList.count() ; bdaIndex++)
	//				 {
	//					 PBDA_TYPE pBdaType = pDaType->bdaTypeList.at(bdaIndex);

	//					 if (QString(pBdaType->name) == tmpBdaName) // .mag
	//					 {
	//						 tmpDaIndex++;
	//					     bFind = true;
	//						 pBdaType1 = pBdaType;
	//						 typeStr = QString(pBdaType->type);
	//						 break;
	//					 }
	//				 }	
	//			 }
	//			 break;
	//		 case 2:
	//			  {
	//				 for (int bdaIndex = 0 ; bdaIndex < pDaType->bdaTypeList.count() ; bdaIndex++)
	//				 {
	//					 PBDA_TYPE pBdaType = pDaType->bdaTypeList.at(bdaIndex);

	//					 if (QString(pBdaType->name) == tmpBdaName1) //.f
	//					 {
	//						 tmpDaIndex++;
	//					     bFind = true;
	//						 pBdaType1 = pBdaType;
	//						 typeStr = QString(pBdaType->type);
	//						 break;
	//					 }
	//				 }
	//			 }
	//			 break;
	//		 }

	//		 if (tmpDaIndex >= daCount)    //到最基本的节点
	//		 {
	//			PCUSTOM_DA pCustDa = new CUSTOM_DA();
	//		    daList.append(pCustDa);

	//			 pCustDa->daName =daName;
	//			 if (pBdaType1)
	//			   pCustDa->daDataType = QString(pBdaType1->bType);
	//			 return true;
	//		 }
	//		 else
	//		 {
	//		    if (bFind)
	//			{
	//				tmpIndex = -1;
	//			}

	//			bFind = false; 
	//		 }
	//			 
	//	 }

	// }
	//}
	//return true;
}



bool  ClientDSE_ScdManagerImpl::makeDaName(PCUSTOM_DA pCustDa,QString daTypeNameForFind)
{
	PDA_TYPE pDaType = m_sta.daTypeListMap.value(daTypeNameForFind);
	if (pDaType)
	{

		QMapIterator<QString, PBDA_TYPE> daTypeListMapIter(pDaType->bdaTypeListMap);
		while (daTypeListMapIter.hasNext()) 
		{
			daTypeListMapIter.next();

			PBDA_TYPE pBdaType = daTypeListMapIter.value();
			if (pBdaType)
			{
				QString bdaTypeNme= QString(pBdaType->name);

				PCUSTOM_DA pCusDa1 = new CUSTOM_DA();
				pCustDa->custDaList.append(pCusDa1);
				pCusDa1->daName = QString("%1%2")
					.arg(pCustDa->daName)
					.arg(pCustDa->daName==""?bdaTypeNme:QString(".%1").arg(bdaTypeNme));

				if (QString(pBdaType->type) != "")
				{
					makeDaName(pCusDa1,QString(pBdaType->type));
				}
				else
				{
					pCusDa1->daDataType = QString(pBdaType->bType);   //基础数据类型
					return true;

				}

			}

		}
	}

	//for (int i = 0 ; i < m_sta.daTypeList.count() ; i++)
	//{
	//  PDA_TYPE pDaType = m_sta.daTypeList.at(i);

	//  if (QString(pDaType->id) != daTypeNameForFind )
	//	 continue;

	//  for (int j = 0 ; j < pDaType->bdaTypeList.count() ; j++)
	//  {
	//   PBDA_TYPE pBdaType = pDaType->bdaTypeList.at(j);

	//   QString bdaTypeNme= QString(pBdaType->name);

	//   PCUSTOM_DA pCusDa1 = new CUSTOM_DA();
	//   pCustDa->custDaList.append(pCusDa1);
	//   pCusDa1->daName = QString("%1%2")
	//	                 .arg(pCustDa->daName)
	//		             .arg(pCustDa->daName==""?bdaTypeNme:QString(".%1").arg(bdaTypeNme));

	//   if (QString(pBdaType->type) != "")
	//   {
	//		makeDaName(pCusDa1,QString(pBdaType->type));
	//   }
	//   else
	//   {
	//		pCusDa1->daDataType = QString(pBdaType->bType);   //基础数据类型
	//		return true;

	//   }
	//  }
	//}

	return false;

}

PDO_TYPE   ClientDSE_ScdManagerImpl:: findDoType(QString id,QString cdc)
{

	QMapIterator<QString, PDO_TYPE> doTypeListIter(m_sta.doTypeListMap);
	while (doTypeListIter.hasNext()) 
	{
		doTypeListIter.next();

		PDO_TYPE pDo = doTypeListIter.value();

		if ( (QString("%1").arg(pDo->id) == id ) && ( QString("%1").arg(pDo->cdc) == cdc) )
		{
			return pDo;
		}

	}
	return NULL;


	/*  for (int i = 0 ; i < m_sta.doTypeList.size() ; i++)
	{
	PDO_TYPE pDo = m_sta.doTypeList[i];

	if ( (QString("%1").arg(pDo->id) == id ) && ( QString("%1").arg(pDo->cdc) == cdc) )
	{
	return pDo;
	}

	}


	return NULL;*/


}

void  ClientDSE_ScdManagerImpl::setlnClassLLN0ChinName(FCDA fcda,QString desc )
{
	if (!m_ied)
		return ;

	for (int i = 0 ; i < m_ied->smvGseList.count()  ; i++)
	{

		PSMVGSE smvgse = m_ied->smvGseList.at(i);

		for (int j = 0 ; j < smvgse->dataSetList.count() ; j++)
		{
			PDATASET dataSet = smvgse->dataSetList.at(j);

			for (int l = 0 ; l < dataSet->fcdaList.count() ; l++)
			{
				PFCDA fcda1 = dataSet->fcdaList.at(l);

				//写dom的名称
				if ( (fcda1->ldInst == fcda.ldInst)  &&  (fcda1->doName == fcda.doName)  )
				{
					fcda1->chnName = desc;
					// fcda1->chnName = QString("%1 %2").arg(fcda.lnDesc).arg(desc);

				}
			}
		}
	}


}

//新增加
bool  ClientDSE_ScdManagerImpl::setSDIChinName(FCDA fcda,QString desc,QString doName)
{
	if ( (!m_ied) || (desc =="") )
		return false;

	bool bRet = false;

	for (int i = 0 ; i < m_ied->smvGseList.count()  ; i++)
	{

		PSMVGSE smvgse = m_ied->smvGseList.at(i);

		for (int j = 0 ; j < smvgse->dataSetList.count() ; j++)
		{
			PDATASET dataSet = smvgse->dataSetList.at(j);

			for (int l = 0 ; l < dataSet->fcdaList.count() ; l++)
			{
				PFCDA fcda1 = dataSet->fcdaList.at(l);			
				//写dom的名称
				if ( (fcda1->ldInst == fcda.ldInst) && (fcda1->prefix == fcda.prefix) && (fcda1->lnInst == fcda.lnInst) && (fcda1->lnClass == fcda.lnClass) && (fcda1->doName == doName)  )
				{
					QString strChnName = fcda1->chnName;

					fcda1->chnName = QString("%1%2%3").arg(fcda.lnDesc).arg(strChnName).arg(desc);
					//fcda1->chnName = desc;
					//fcda1->chnName = QString("%1 %2").arg(fcda.lnDesc).arg(desc);

					bRet = true;

				}				
			}
		}
	}


	return bRet;


}


void  ClientDSE_ScdManagerImpl::setChinName(FCDA fcda,QString desc,QString doName,QString daName)
{
	if (!m_ied)
		return ;


	for (int i = 0 ; i < m_ied->smvGseList.count()  ; i++)
	{

		PSMVGSE smvgse = m_ied->smvGseList.at(i);

		for (int j = 0 ; j < smvgse->dataSetList.count() ; j++)
		{
			PDATASET dataSet = smvgse->dataSetList.at(j);

			for (int l = 0 ; l < dataSet->fcdaList.count() ; l++)
			{
				PFCDA fcda1 = dataSet->fcdaList.at(l);

				if (daName == "")
				{

					QString tmpDoName,tmpSdoName;

					tmpDoName = fcda1->doName;
					QStringList strDOList = fcda1->doName.split(".");  //doName="PhV.phsA"
					if (strDOList.count() >1)
					{
						tmpDoName = strDOList.at(0);        //PhV
						tmpSdoName = strDOList.at(1);       //phsA
					}

					//写dom的名称
					if ( (fcda1->ldInst == fcda.ldInst) && (fcda1->prefix == fcda.prefix) && (fcda1->lnInst == fcda.lnInst) && (fcda1->lnClass == fcda.lnClass) && (tmpDoName == doName)  )
					{
						fcda1->chnName = desc;
						// fcda1->chnName = QString("%1 %2").arg(fcda.lnDesc).arg(desc);

					}

				}
				else
				{
					//写ai的名称
					if ( (fcda1->ldInst == fcda.ldInst) && (fcda1->prefix == fcda.prefix) && (fcda1->lnInst == fcda.lnInst) && (fcda1->lnClass == fcda.lnClass) && (fcda1->doName == doName)  && (fcda1->daName == daName))
					{
						fcda1->chnName = desc;
						//fcda1->chnName = QString("%1 %2").arg(fcda.lnDesc).arg(desc);

					}
				}




			}
		}
	}

}



void ClientDSE_ScdManagerImpl::clearStation()
{

	for (int i = 0 ; i < m_sta.subNetList.count() ;i++)
	{
		PCUSTOM_SUBNET pSub = m_sta.subNetList[i];


		for (int j = 0 ;j < pSub->iedList.size() ; j++)
		{
			PINTERNAL_IED ied = pSub->iedList.at(j);

			for (int m = 0 ; m < ied->mmsList.size() ;m++)
			{
				PMMSINFO pMms =  ied->mmsList[m];

				delete pMms;
			}
			ied->mmsList.clear();

			for (int m = 0 ; m < ied->smvGseList.count() ;m++)
			{
				PSMVGSE1 pSmg = ied->smvGseList[m];

				delete pSmg;
			}
			ied->smvGseList.clear();

			delete ied;

		}

		pSub->iedList.clear();
		delete pSub;

	}

	m_sta.subNetList.clear();


	for (int i =0 ; i < m_sta.iedList.count() ; i++)
	{
		PINTERNAL_IED1 ied = m_sta.iedList[i];

		//清除smv、goose控制块
		for (int m = 0 ; m < ied->smvGseList.count() ; m++)
		{

			PSMVGSE smvgse = ied->smvGseList.at(m);

			if (!smvgse)
				continue;

			for (int l = 0 ; l < smvgse->dataSetList.count() ; l++)
			{
				PDATASET dataset = smvgse->dataSetList.at(l);

				if (!dataset)
					continue;

				for (int n = 0  ; n < dataset->fcdaList.count() ; n++)
				{

					PFCDA fcda = dataset->fcdaList.at(n);
					if (!fcda)
						continue;

					delete fcda;
				}

				dataset->fcdaList.clear();
				delete dataset;

			}	
			smvgse->dataSetList.clear();
			delete smvgse;
		}
		ied->smvGseList.clear();
	}


	QMapIterator<QString, PLNODE_TYPE> lnTypeListIter(m_sta.lnTypeListMap);
	while (lnTypeListIter.hasNext()) 
	{
		lnTypeListIter.next();

		PLNODE_TYPE pLn = lnTypeListIter.value();
		if (pLn)
			delete pLn;
	}
	m_sta.lnTypeListMap.clear();


	QMapIterator<QString, PDO_TYPE> doTypeListIter(m_sta.doTypeListMap);
	while (doTypeListIter.hasNext()) 
	{
		doTypeListIter.next();

		PDO_TYPE pDoType = doTypeListIter.value();
		if (pDoType)
		{
			for (int i = 0 ;i < pDoType->daList.size()  ; i++)
			{
				PDA pDa = pDoType->daList.at(i);
				if (pDa) delete pDa;

			}
			pDoType->daList.clear();

			QMapIterator<QString, PSDO> sdoListIter1(pDoType->sdoListMap);
			while (sdoListIter1.hasNext()) 
			{
				sdoListIter1.next();

				PSDO  pSdo = sdoListIter1.value();
				if (pSdo) delete pSdo;
			}
			pDoType->sdoListMap.clear();


			delete pDoType;

		}
	}
	m_sta.doTypeListMap.clear();


	QMapIterator<QString, PDA_TYPE> daTypeListIter(m_sta.daTypeListMap);
	while (daTypeListIter.hasNext()) 
	{
		daTypeListIter.next();

		PDA_TYPE pDaType = daTypeListIter.value();
		if (pDaType)
		{
			QMapIterator<QString, PBDA_TYPE> bdaTypeListIter(pDaType->bdaTypeListMap);
			while (bdaTypeListIter.hasNext()) 
			{
				bdaTypeListIter.next();

				PBDA_TYPE pBdaType = bdaTypeListIter.value();
				if (pBdaType) delete pBdaType;
			}
			pDaType->bdaTypeListMap.clear();


			delete pDaType;
		}
	}
	m_sta.daTypeListMap.clear();

	/*for (int i = 0 ;i < m_sta.lnTypeList.count() ;i++)
	{
	PLNODE_TYPE pLn = m_sta.lnTypeList[i];

	for (int j = 0 ; j < pLn->doList.count() ; j++)
	{

	PDO pDo = pLn->doList[j];

	delete pDo;

	}

	pLn->doList.clear();

	delete pLn;
	}

	m_sta.lnTypeList.clear();


	for (int i = 0 ;i < m_sta.doTypeList.count() ; i++)
	{

	PDO_TYPE pDoType = m_sta.doTypeList[i];

	for (int j = 0 ; j < pDoType->daList.count() ; j++)
	{

	PDA pDaType = pDoType->daList[j];

	delete pDaType;
	}

	pDoType->daList.clear();



	for (int j = 0 ;j < pDoType->sdoList.count() ;j++)
	{
	PSDO pSdo = pDoType->sdoList[j];

	delete pSdo;
	}
	pDoType->sdoList.clear();

	delete pDoType;



	}

	m_sta.doTypeList.clear();


	for (int  i = 0 ; i < m_sta.daTypeList.count() ; i++)
	{

	PDA_TYPE pDaType = m_sta.daTypeList[i];

	for (int j = 0 ; j < pDaType->bdaTypeList.count() ;j++)
	{

	PBDA_TYPE pBdaType = pDaType->bdaTypeList[j];

	delete pBdaType;
	}

	pDaType->bdaTypeList.clear();
	delete pDaType;
	}

	m_sta.daTypeList.clear();*/


}

void ClientDSE_ScdManagerImpl::createXMlNode(QDomDocument &doc)
{
	QStringList strList;
	QDomText    text;
	QDomElement element;
	QDomProcessingInstruction instruction;

	instruction = doc.createProcessingInstruction("xml","version=\'1.0\' encoding=\'GBK\'");
	doc.appendChild(instruction);

	//cfg
	QDomElement rootElem = doc.createElement("Cfg");
	doc.appendChild(rootElem); 


	if (m_subSta)
	{
		strList.clear();
		strList  << QString("name")   << QString(m_subSta->name);

		QDomElement subStaElem = createNode(doc,rootElem,QString("SubSta"),strList);
	}


	//添加所有子网络
	strList.clear();
	QDomElement networkElem;

	networkElem = createNode(doc,rootElem,QString("NetWork"),strList);

	for (int i = 0 ;i < m_subNetList->size(); i++)
	{
		PSUBNET pSubNet = (*m_subNetList)[i];

		strList.clear();
		strList  << QString("type")   << QString("%1").arg(pSubNet->type) 
			<< QString("desc")   << QString("%1").arg(pSubNet->desc)
			<< QString("name")   << QString("%1").arg(pSubNet->name);

		QDomElement subNetElem = createNode(doc,networkElem,QString("SubNet"),strList);

		for (int j = 0 ; j < pSubNet->iedList.size() ; j++)
		{

			PIED pIed =  pSubNet->iedList[j];
			strList.clear();
			strList  << QString("type")			  << QString("%1").arg(pIed->type) 
				<< QString("desc")		      << QString("%1").arg(pIed->desc)
				<< QString("name")			  << QString("%1").arg(pIed->name)
				<< QString("manufacturer")   << QString("%1").arg(pIed->manufacturer)
				<< QString("actSG")		  << QString("%1").arg(pIed->settingCtrl.actSG)
				<< QString("numOfSGs")        << QString("%1").arg(pIed->settingCtrl.numOfSGs);

			//添加单个设备
			QDomElement iedElem = createNode(doc,subNetElem,QString("Ied"),strList);

			//添加所有goose控制块
			for (t_uint l = 0 ; l < pIed->gsList.size() ;l++)
			{
				PGOOSEINFO pGoosInfo = pIed->gsList[l];

				strList.clear();
				strList  /*<< QString("customId")  << QString("%1").arg(pGoosInfo->customId) 
						 << QString("ethIndex")	 << QString("%1").arg(pGoosInfo->ethIndex)
						 << QString("appid")	 << QString("%1").arg(pGoosInfo->appid)
						 << QString("prefix")	 << QString("%1").arg(pGoosInfo->prefix)
						 << QString("mac")       << QString("%1").arg(pGoosInfo->mac)
						 << QString("confRev")   << QString("%1").arg(pGoosInfo->confRev)
						 << QString("gcRef")     << QString("%1").arg(pGoosInfo->gcRef)*/
						 << QString("datSet")    << QString("%1").arg(pGoosInfo->datSet)

						 << QString("datCount")  << QString("%1").arg(pGoosInfo->datCount)
						 /* << QString("vLan")      << QString("%1").arg(pGoosInfo->vlan)
						 << QString("goId")      << QString("%1").arg(pGoosInfo->goId)
						 << QString("t0")        << QString("%1").arg(pGoosInfo->t0)
						 << QString("t1")        << QString("%1").arg(pGoosInfo->t1)
						 << QString("enable")    << QString("%1").arg(pGoosInfo->enable)*/
						 << QString("attribId")  << QString("%1").arg(pGoosInfo->attribId)
						 << QString("rpt_name")  << QString("%1").arg(pGoosInfo->reportCtrl.name)
						 << QString("rpt_datSet")  << QString("%1").arg(pGoosInfo->reportCtrl.datSet)
						 << QString("rpt_rptID")  << QString("%1").arg(pGoosInfo->reportCtrl.rptID)
						 << QString("rpt_max")  << QString("%1").arg(pGoosInfo->reportCtrl.max);
				//添加单个goose控制块
				QDomElement gooseElem = createNode(doc,iedElem,QString("Goose"),strList);

				//添加所有通道信息
				for (t_uint m= 0 ; m <pGoosInfo->chnList.size() ;m++ )
				{
					PGSCHNINFO pGsChnInfo = pGoosInfo->chnList[m];

					strList.clear();
					strList  << QString("index")     << QString("%1").arg(pGsChnInfo->index) 
						<< QString("chnName")	 << QString("%1").arg(pGsChnInfo->chnName)
						/* << QString("gstype_bStruct") << QString("%1").arg(pGsChnInfo->dataType.bStruct)
						<< QString("gstype_fromatValue") << QString("%1").arg(pGsChnInfo->dataType.fromatValue)
						<< QString("gstype_type") << QString("%1").arg(pGsChnInfo->dataType.type)*/
						<< QString("dataType")     << QString("%1").arg(pGsChnInfo->dataType)
						<< QString("dataSize")     << QString("%1").arg(pGsChnInfo->dataSize)
						<< QString("dataTypeStr")  << QString("%1").arg(pGsChnInfo->dataTypeStr)
						<< QString("datSetName")   << QString("%1").arg(pGsChnInfo->datSetName)
						<< QString("strVal")       << QString("%1").arg(pGsChnInfo->strVal);
					/*<< QString("type")      << QString("%1").arg(pGsChnInfo->type)
					<< QString("flag")      << QString("%1").arg(pGsChnInfo->flag)
					<< QString("enable")    << QString("%1").arg(pGsChnInfo->enable);*/

					createNode(doc,gooseElem,QString("ChnInfo"),strList);
				}
			}

			//添加所有smv控制块
			for (t_uint l = 0 ; l < pIed->svList.size() ;l++)
			{
				PSVINFO pSvInfo = pIed->svList[l];

				strList.clear();
				strList  /*<< QString("customId")  << QString("%1").arg(pSvInfo->customId) 
						 << QString("ethIndex")	 << QString("%1").arg(pSvInfo->ethIndex)
						 << QString("appid")	 << QString("%1").arg(pSvInfo->appid)
						 << QString("prefix")	 << QString("%1").arg(pSvInfo->prefix)
						 << QString("mac")       << QString("%1").arg(pSvInfo->mac)
						 << QString("confRev")   << QString("%1").arg(pSvInfo->confRev)
						 << QString("svId")      << QString("%1").arg(pSvInfo->svId)*/
						 << QString("datSet")    << QString("%1").arg(pSvInfo->datSet)
						 // << QString("ldDevName")    << QString("%1").arg(pSvInfo->ldDevName)
						 // << QString("lnName")       << QString("%1").arg(pSvInfo->lnName)
						 << QString("datCount")  << QString("%1").arg(pSvInfo->datCount)
						 /* << QString("datSetType")<< QString("%1").arg(pSvInfo->datSetType)
						 << QString("convertType")<< QString("%1").arg(pSvInfo->convertType)
						 << QString("vLan")      << QString("%1").arg(pSvInfo->vlan)
						 << QString("enable")     << QString("%1").arg(pSvInfo->enable)*/
						 << QString("attribId")     << QString("%1").arg(pSvInfo->attribId);
				//添加单个SMV控制块
				QDomElement smvElem = createNode(doc,iedElem,QString("Smv"),strList);

				// //添加AV_INFO
				// strList.clear();
				// strList  << QString("priRated")   << QString("%1").arg(pSvInfo->curInfo.priRated) 
				//	<< QString("secRated")	 << QString("%1").arg(pSvInfo->curInfo.secRated)
				//	<< QString("priReferVal")<< QString("%1").arg(pSvInfo->curInfo.priReferVal)
				//	<< QString("referToSamp")<< QString("%1").arg(pSvInfo->curInfo.referToSamp)
				//	<< QString("overLaod")   << QString("%1").arg(pSvInfo->curInfo.overLaod)
				//	<< QString("sampLimit")  << QString("%1").arg(pSvInfo->curInfo.sampLimit)
				//	<< QString("twoADDiff")  << QString("%1").arg(pSvInfo->curInfo.twoADDiff)
				//    << QString("precipValue")<< QString("%1").arg(pSvInfo->curInfo.precipValue);
				////添加电流AV_INFO
				//createNode(doc,smvElem,QString("CurInfo"),strList);


				// strList.clear();
				// strList  << QString("priRated")   << QString("%1").arg(pSvInfo->volInfo.priRated) 
				//	<< QString("secRated")	 << QString("%1").arg(pSvInfo->volInfo.secRated)
				//	<< QString("priReferVal")<< QString("%1").arg(pSvInfo->volInfo.priReferVal)
				//	<< QString("referToSamp")<< QString("%1").arg(pSvInfo->volInfo.referToSamp)
				//	<< QString("overLaod")   << QString("%1").arg(pSvInfo->volInfo.overLaod)
				//	<< QString("sampLimit")  << QString("%1").arg(pSvInfo->volInfo.sampLimit)
				//	<< QString("twoADDiff")  << QString("%1").arg(pSvInfo->volInfo.twoADDiff)
				//          << QString("precipValue")<< QString("%1").arg(pSvInfo->volInfo.precipValue);
				////添加电压AV_INFO
				//createNode(doc,smvElem,QString("VolInfo"),strList);

				//添加通道信息
				for (t_uint m = 0 ; m < pSvInfo->chnList.size() ;m++)
				{
					PSVCHNINFO pSvChnInfo =  pSvInfo->chnList[m];

					strList.clear();
					strList  << QString("index")   << QString("%1").arg(pSvChnInfo->index) 
						// << QString("remoteIndex") << QString("%1").arg(pSvChnInfo->remoteIndex)
						<< QString("chnName")	  << QString("%1").arg(pSvChnInfo->chnName)
						<< QString("datSetName")  << QString("%1").arg(pSvChnInfo->datSetName)
						/*<< QString("type")        << QString("%1").arg(pSvChnInfo->type)
						<< QString("flag")        << QString("%1").arg(pSvChnInfo->flag)
						<< QString("priRated")    << QString("%1").arg(pSvChnInfo->priRated)
						<< QString("secRated")    << QString("%1").arg(pSvChnInfo->secRated)
						<< QString("priReferVal") << QString("%1").arg(pSvChnInfo->priReferVal)
						<< QString("referToSamp") << QString("%1").arg(pSvChnInfo->referToSamp)
						<< QString("overLaod")    << QString("%1").arg(pSvChnInfo->overLaod)
						<< QString("sampLimit")   << QString("%1").arg(pSvChnInfo->sampLimit)
						<< QString("twoADDiff")   << QString("%1").arg(pSvChnInfo->twoADDiff)
						<< QString("range")       << QString("%1").arg(pSvChnInfo->range)
						<< QString("blm")         << QString("%1").arg(pSvChnInfo->blm)
						<< QString("smpMin")      << QString("%1").arg(pSvChnInfo->smpMin)
						<< QString("freq")        << QString("%1").arg(pSvChnInfo->freq)
						<< QString("enable")      << QString("%1").arg(pSvChnInfo->enable)
						<< QString("precipValue") << QString("%1").arg(pSvChnInfo->precipValue)*/
						<< QString("dataType")  << QString("%1").arg(pSvChnInfo->dataType)
						<< QString("dataSize")  << QString("%1").arg(pSvChnInfo->dataSize)
						<< QString("dataTypeStr")  << QString("%1").arg(pSvChnInfo->dataTypeStr)
						<< QString("maxVal")      << QString("%1").arg(pSvChnInfo->maxVal)
						<< QString("minVal")      << QString("%1").arg(pSvChnInfo->minVal)
						<< QString("stepVal")     << QString("%1").arg(pSvChnInfo->stepVal)
						<< QString("unitStr")     << QString("%1").arg(pSvChnInfo->unitStr)
						<< QString("strVal")      << QString("%1").arg(pSvChnInfo->strVal);

					//添加单个通道信息
					createNode(doc,smvElem,QString("ChnInfo"),strList);
				}

			}

			for (int l = 0 ; l < pIed->mmsList.size() ;l++)
			{

				PMMSINFO pMms = pIed->mmsList[l];

				strList.clear();
				strList  << QString("name")    << QString("%1").arg(pMms->name) 
					<< QString("desc")    << QString("%1").arg(pMms->desc) 
					<< QString("ip")    << QString("%1").arg(pMms->ip) 
					<< QString("apName")    << QString("%1").arg(pMms->apName) 
					<< QString("mask")	 << QString("%1").arg(pMms->mask)
					<< QString("destIp")    << QString("%1").arg(pMms->destIp) 
					<< QString("destMask")    << QString("%1").arg(pMms->destMask) 
					<< QString("appid") << QString("%1").arg(pMms->appid)
					<< QString("ethIndex") << QString("%1").arg(pMms->ethIndex)
					<< QString("overtime") << QString("%1").arg(pMms->overtime);
				//添加单个mms控制块
				QDomElement mmsElem = createNode(doc,iedElem,QString("Mms"),strList);


			}

			for (int l = 0 ; l < pIed->ptpList.size() ;l++)
			{

				PPTPINFO pPtp = pIed->ptpList[l];

				strList.clear();
				strList  << QString("name")    << QString("%1").arg(pPtp->name) 
					<< QString("desc")    << QString("%1").arg(pPtp->desc) 
					<< QString("appid") << QString("%1").arg(pPtp->appid)
					<< QString("ethIndex") << QString("%1").arg(pPtp->ethIndex);
				//添加单个mms控制块
				QDomElement ptpElem = createNode(doc,iedElem,QString("Ptp"),strList);


			}
		}
	}

}


PINTERNAL_IED  ClientDSE_ScdManagerImpl::getCommLayerIed(QString iedName)
{

	for (int netIndex = 0 ; netIndex < m_sta.subNetList.count() ; netIndex++)
	{
		PCUSTOM_SUBNET pSubNet = m_sta.subNetList[netIndex];

		for (int iedIndex = 0 ; iedIndex < pSubNet->iedList.size() ;iedIndex++ )
		{
			PINTERNAL_IED pIed = pSubNet->iedList[iedIndex];

			if (pIed->iedName == iedName)
				return pIed;
		}
	}

	return NULL;

}

void ClientDSE_ScdManagerImpl::copyFrom(MMSINFO &dest,MMSINFO &src)
{

	strcpy(dest.ip,src.ip);
	strcpy(dest.mask,src.mask);
	strcpy(dest.destIp,src.destIp);
	strcpy(dest.destMask,src.destMask);
	strcpy(dest.apName,src.apName);
	dest.appid = src.appid;
	dest.ethIndex = src.ethIndex;
	dest.overtime = src.overtime;

}

void   ClientDSE_ScdManagerImpl::makeScdIp()
{

	t_int gsmvIndex = 1;
	t_short iedId = 1;
	t_int mmsAppId = 1;


	//进度指示
	int  tmpFreeSpace = m_fileProgress.fileTotalLen-m_fileProgress.fileLengthed;
	int  tmpStep = 0;
	if (tmpFreeSpace>0)
	{
		tmpStep = tmpFreeSpace/(m_equipTotal+1);  //1是多增加一个装置，解析完再显示成100%
	}

	if (m_subSta)
	{
		sprintf(m_subSta->name,"%s",m_sta.name.toAscii().data());  //子站名称
	}

	for (int subNetIndex = 0 ; subNetIndex < m_sta.subNetList.count() ; subNetIndex++)
	{
		PCUSTOM_SUBNET pCustSubNet = m_sta.subNetList[subNetIndex];

		PSUBNET pSubNet = new SUBNET();
		m_subNetList->push_back(pSubNet);
		strcpy(pSubNet->type,pCustSubNet->type);
		strcpy(pSubNet->name,pCustSubNet->name);
		strcpy(pSubNet->desc,pCustSubNet->desc);




		for (int iedIndex = 0 ;iedIndex < pCustSubNet->iedList.size();iedIndex++)
		{
			PINTERNAL_IED pIntIed = pCustSubNet->iedList[iedIndex];



			//进度指示
			m_fileProgress.fileLengthed += tmpStep;

			CMD_TYPE_DEF::CMD cmd;

			cmd.head.cmd = CMD_TYPE_DEF::CMD_FILE_PROGRESSS;
			memcpy(cmd.data.str,&m_fileProgress,sizeof(CMD_TYPE_DEF::FILE_PROGRESS));
			//回调，传送的文件已经接收的文件长度
			m_routine(m_param,cmd);


			PIED pIed = new IED();
			//添加设备
			pSubNet->iedList.push_back(pIed);
			strcpy( pIed->name,pIntIed->iedName.toAscii().data());
			strcpy( pIed->desc,pIntIed->desc.toAscii().data());
			strcpy(pIed->type,pIntIed->apName.toAscii().data());
			strcpy(pIed->apName,pIntIed->apName.toAscii().data());


			for (int j= 0 ;j < pIntIed->mmsList.size() ;j++)
			{

				PMMSINFO pMms  =  pIntIed->mmsList[j];

				PMMSINFO pMms1 = new MMSINFO();
				pIed->mmsList.push_back(pMms1);

				sprintf(pMms1->ip,"%s",pMms->ip);
				sprintf(pMms1->mask,"%s",pMms->mask);

				pMms1->appid = ((CMD_TYPE_DEF::MT_MMS  & 0xff) << 16)+ mmsAppId++;
			}


		}

	}

}

void ClientDSE_ScdManagerImpl:: makeScdInf()
{

	t_int gsmvIndex = 1;
	t_short iedId = 1;
	t_int mmsAppId = 1;

	QDateTime dD = QDateTime::currentDateTime();

	QString  str= dD.toString("yyyy/MM/dd HH:mm:ss.zzz");

	printf("makeScdInf:%s begn\n",str.toAscii().data());

	sprintf(m_subSta->name,"%s",m_sta.name.toAscii().data());


	for (int subNetIndex = 0 ; subNetIndex < m_sta.subNetList.count() ; subNetIndex++)
	{
		PCUSTOM_SUBNET pCustSubNet = m_sta.subNetList[subNetIndex];

		PSUBNET pSubNet = new SUBNET();
		m_subNetList->push_back(pSubNet);
		strcpy(pSubNet->type,pCustSubNet->type);
		strcpy(pSubNet->name,pCustSubNet->name);
		strcpy(pSubNet->desc,pCustSubNet->desc);

		for (int iedIndex = 0 ;iedIndex < pCustSubNet->iedList.size();iedIndex++)
		{
			PINTERNAL_IED pIntIed = pCustSubNet->iedList[iedIndex];


			PIED pIed = new IED();
			//添加设备
			pSubNet->iedList.push_back(pIed);
			strcpy( pIed->name,pIntIed->iedName.toAscii().data());
			strcpy( pIed->desc,pIntIed->desc.toAscii().data());
			strcpy(pIed->type,pIntIed->apName.toAscii().data());
			//为模拟服务器增加定值控制块
			pIed->settingCtrl.actSG     = m_settingCtrl.actSG;
			pIed->settingCtrl.numOfSGs  = m_settingCtrl.numOfSGs;

			for (int j= 0 ;j < pIntIed->mmsList.size() ;j++)
			{

				PMMSINFO pMms  =  pIntIed->mmsList[j];

				PMMSINFO pMms1 = new MMSINFO();
				pIed->mmsList.push_back(pMms1);

				sprintf(pMms1->ip,"%s",pMms->ip);
				sprintf(pMms1->mask,"%s",pMms->mask);

				pMms1->appid = ((CMD_TYPE_DEF::MT_MMS  & 0xff) << 16)+ mmsAppId++;
			}



			PINTERNAL_IED1 pInternalIed = findIED(pIntIed->iedName);
			if (pInternalIed)
			{
				strcpy( pIed->manufacturer,pInternalIed->manufacturer.toAscii().data());
				strcpy( pIed->desc,pInternalIed->desc.toAscii().data());

				for (t_uint16 smvGseIndex = 0 ; smvGseIndex< pInternalIed->smvGseList.count() ; smvGseIndex++)
				{
					PSMVGSE  pSmvgse= pInternalIed->smvGseList[smvGseIndex];

					if ( (pSmvgse->control == ATTRIB_SWI)         ||  
						(pSmvgse->control == ATTRIB_YB)          || 
						(pSmvgse->control == ATTRIB_YYB)         || 
						(pSmvgse->control == ATTRIB_ALARM)       || 
						(pSmvgse->control == ATTRIB_SELF_CHECK)  || 
						(pSmvgse->control == ATTRIB_RELAYREC)  || 
						(pSmvgse->control == ATTRIB_EVENT)
						)
					{

						int  gsIndex = 1;
						PGOOSEINFO pGooseInfo = new GOOSEINFO();
						//添加goose控制块
						pIed->gsList.push_back(pGooseInfo);

						pGooseInfo->attribId = pSmvgse->control;

						pGooseInfo->customId = gsmvIndex++;
						pGooseInfo->ethIndex = 0;
						strcpy(pGooseInfo->prefix,pInternalIed->desc.toAscii().data());
						pGooseInfo->confRev = pSmvgse->confRev.toUInt();                                         //1~N 数据集改变次数  ?????   
						QString gcRef = QString("%1%2/LLN0$GO$%3")
							.arg(pInternalIed->iedName)
							.arg(pSmvgse->inst)
							.arg(pSmvgse->cbName);
						strcpy(pGooseInfo->gcRef,gcRef.toAscii().data());
						QString datSet = QString("%1%2/LLN0$%3")
							.arg(pInternalIed->iedName)
							.arg(pSmvgse->inst)
							.arg(pSmvgse->datSet);
						strcpy(pGooseInfo->datSet,datSet.toAscii().data());   
						pGooseInfo->datCount =0;										  //数据对象数目
						pGooseInfo->enable = 0;                                           //0位：是否勾选 1位：是否检查数据对象数目 2位：是否检查confRev字段 3位：检查gcRef字段 4位：检查datSet字段 5位：需要暂态录波,6:是否判AB网     


						for (int m = 0 ;m < pSmvgse->dataSetList.count() ;m++)
						{
							PDATASET pDataSet = pSmvgse->dataSetList.at(m);

							for (int l= 0 ; l < pDataSet->fcdaList.count() ; l++)
							{
								PFCDA pFcda = pDataSet->fcdaList.at(l);
								if (1)
								{

									t_char   strDefaultVal[MAX_STRLEN]; //默认值

									t_bool bRet = false;
									QList<PCUSTOM_DA> daList;
									QList<QString> strList;
									getDataType(pFcda,daList);
									getDaName(daList,strList);
									freeDaName(daList);

									for (int strListIndex = 0 ; strListIndex < strList.count(); strListIndex++)
									{
										QString strDaAType = strList.at(strListIndex);
										QStringList daATypeList = strDaAType.split("/"); 
										if (daATypeList.count() <2)
											continue;

										QString daName = daATypeList.at(0);
										QString daType = daATypeList.at(1);


										GSCHNINFO *pgseChnInfo = new GSCHNINFO();
										pGooseInfo->chnList.push_back(pgseChnInfo);  //添加goose通道信息

										pgseChnInfo->index = gsIndex++;
										strcpy(pgseChnInfo->chnName,pFcda->chnName.toAscii().data());        //通道名称

										/*pgseChnInfo->dataType = getDataTypeValue(daType.isEmpty()?"SPS":daType,strDefaultVal);
										pgseChnInfo->dataSize = getDataSizeValue(pgseChnInfo->dataType);
										sprintf(pgseChnInfo->dataTypeStr,"%s",daType.isEmpty()?"SPS":daType.toAscii().data());
										strcpy(pgseChnInfo->strVal,strDefaultVal);*/

										QString datSetName = QString("%1%2/%3%4%5$%6$%7%8")
											.arg(pIed->name)
											.arg(pFcda->ldInst)
											.arg(pFcda->prefix)
											.arg(pFcda->lnClass)
											.arg(pFcda->lnInst)
											.arg(pFcda->fc)
											.arg(pFcda->doName)
											.arg(daName.isEmpty() ? "":QString("$%1").arg(daName) );

										datSetName.replace(".","$");
										sprintf(pgseChnInfo->datSetName,"%s",datSetName.toAscii().data());

									}
								}							

							}

							pGooseInfo->datCount += pDataSet->fcdaList.count();
						}

					}
					else if ( (pSmvgse->control == ATTRIB_SETTING) ||  (pSmvgse->control == ATTRIB_EQU_PARAM)   || (pSmvgse->control == ATTRIB_SAMPLE) )
					{
						PSVINFO pSvInfo = new SVINFO();
						//添加smv控制块
						pIed->svList.push_back(pSvInfo);
						int  svIndex = 1;

						pSvInfo->attribId = pSmvgse->control;
						pSvInfo->customId = gsmvIndex++;
						pSvInfo->ethIndex = 0;
						strcpy(pSvInfo->prefix,pInternalIed->desc.toAscii().data());
						pSvInfo->confRev = pSmvgse->confRev.toUInt();                                           //1~N 数据集改变次数  ?????   
						strcpy(pSvInfo->svId,pSmvgse->sgAppid.toAscii().data());
						strcpy(pSvInfo->datSet,"");
						pSvInfo->datCount = 0;              //数据对象数目
						pSvInfo->datSetType = 0;
						QString datSet = QString("%1%2/LLN0$%3")
							.arg(pInternalIed->iedName)
							.arg(pSmvgse->inst)
							.arg(pSmvgse->datSet);
						strcpy(pSvInfo->datSet,datSet.toAscii().data()); 

						for (int m = 0 ;m < pSmvgse->dataSetList.count() ;m++)
						{
							PDATASET pDataSet = pSmvgse->dataSetList.at(m);

							for (int l= 0 ; l < pDataSet->fcdaList.count() ; l++)
							{
								PFCDA pFcda = pDataSet->fcdaList.at(l);

								if (1)
								{
									t_char   strDefaultVal[MAX_STRLEN]; //默认值
									t_bool bRet = false;
									QList<PCUSTOM_DA> daList;
									QList<QString> strList;
									getDataType(pFcda,daList);
									getDaName(daList,strList);
									freeDaName(daList);

									for (int strListIndex = 0 ; strListIndex < strList.count(); strListIndex++)
									{
										QString strDaAType = strList.at(strListIndex);
										QStringList daATypeList = strDaAType.split("/"); 
										if (daATypeList.count() <2)
											continue;

										QString daName = daATypeList.at(0);
										QString daType = daATypeList.at(1);

										SVCHNINFO *psvChnInfo = new SVCHNINFO();
										pSvInfo->chnList.push_back(psvChnInfo); //添加通道信息

										psvChnInfo->index  = svIndex++;                            //序号（1~N）
										strcpy(psvChnInfo->chnName,pFcda->chnName.toAscii().data());        //通道名称


										psvChnInfo->maxVal = pFcda->maxVal.toFloat();
										psvChnInfo->minVal = pFcda->minVal.toFloat();
										psvChnInfo->stepVal = pFcda->stepVal.toFloat();
										sprintf(psvChnInfo->unitStr,"%s",pFcda->unitVal.toAscii().data());

										/*psvChnInfo->dataType = getDataTypeValue(daType,strDefaultVal);
										psvChnInfo->dataSize = getDataSizeValue(psvChnInfo->dataType);
										sprintf(psvChnInfo->dataTypeStr,"%s",daType.toAscii().data());
										if (pSvInfo->attribId == 20)
										{
										QString strVal="";
										QString strSplite="";
										strcpy(psvChnInfo->strVal,strVal.toAscii().data());
										}
										else
										{
										strcpy(psvChnInfo->strVal,strDefaultVal);
										}*/


										QString datSetName = QString("%1%2/%3%4%5$%6$%7%8")
											.arg(pIed->name)
											.arg(pFcda->ldInst)
											.arg(pFcda->prefix)
											.arg(pFcda->lnClass)
											.arg(pFcda->lnInst)
											.arg(pFcda->fc)
											.arg(pFcda->doName)
											.arg(daName.isEmpty() ? "":QString("$%1").arg(daName) );

										datSetName.replace(".","$");

										if (datSetName.contains("ang") == true)
										{

											sprintf(psvChnInfo->unitStr,"deg");
											QString strChnName = QString("%1(角度)").arg(psvChnInfo->chnName);
											sprintf(psvChnInfo->chnName,"%s",strChnName.toAscii().data());
										}

										strcpy(psvChnInfo->datSetName,datSetName.toAscii().data());                 //数据对象名


									}	
								}
							}

							pSvInfo->datCount += pDataSet->fcdaList.count();
						}
					}

				}
			}

		}

	}

	dD = QDateTime::currentDateTime();

	str= dD.toString("yyyy/MM/dd HH:mm:ss.zzz");

	printf("makeScdInf:%s end\n",str.toAscii().data());

}



void   ClientDSE_ScdManagerImpl::makeScdInf2()
{

	QStringList strList;
	t_uint8     strCount = 0;
	t_bool      bRet = false;
	t_int       mmsAppId = 1;
	//生成一个子网络节点
	PSUBNET pSubNet = new SUBNET();
	m_subNetList->push_back(pSubNet);

	//进度指示
	int  tmpFreeSpace = m_fileProgress.fileTotalLen-m_fileProgress.fileLengthed;
	int  tmpStep = 0;
	if (tmpFreeSpace>0)
	{
		tmpStep = tmpFreeSpace/(m_equipTotal+1);  //1是多增加一个装置，解析完再显示成100%
	}

	if (m_subSta)
	{
		sprintf(m_subSta->name,"%s",m_sta.name.toAscii().data());  //子站名称
	}


	for (int iedIndex = 0 ;iedIndex < m_sta.iedList.count() ;iedIndex++)
	{
		PINTERNAL_IED1 pIed = m_sta.iedList[iedIndex];


		PIED pIed1 = new IED();
		pSubNet->iedList.push_back(pIed1);  //创建装置


		strcpy( pIed1->name,pIed->iedName.toAscii().data());
		strcpy( pIed1->desc,pIed->desc.toAscii().data());
		strcpy(pIed1->type,pIed->apName.toAscii().data());

		//为模拟服务器增加定值控制块
		pIed1->settingCtrl.actSG     = 1;//m_settingCtrl.actSG;
		pIed1->settingCtrl.numOfSGs  = m_settingCtrl.numOfSGs;


		//进度指示
		m_fileProgress.fileLengthed += tmpStep;


		//回调，传送的文件已经接收的文件长度
		if (m_param)
		{
			CMD_TYPE_DEF::CMD cmd;

			cmd.head.cmd = CMD_TYPE_DEF::CMD_FILE_PROGRESSS;
			memcpy(cmd.data.str,&m_fileProgress,sizeof(CMD_TYPE_DEF::FILE_PROGRESS));
			m_routine(m_param,cmd);
		}




		//添加mms节点
		PINTERNAL_IED pComLayerIed = getCommLayerIed(pIed1->name);

		if (pComLayerIed)
		{
			for (int mmsIndex = 0 ; mmsIndex < pComLayerIed->mmsList.size() ; mmsIndex++)
			{
				MMSINFO *pMmsInfo = pComLayerIed->mmsList[mmsIndex];

				MMSINFO *pMmsInfo1 = new MMSINFO();
				pIed1->mmsList.push_back(pMmsInfo1);

				sprintf(pMmsInfo1->ip,"%s",pMmsInfo->ip);
				sprintf(pMmsInfo1->mask,"%s",pMmsInfo->mask);

				pMmsInfo1->appid = ((CMD_TYPE_DEF::MT_MMS  & 0xff) << 16)+ mmsAppId++;

				copyFrom(*pMmsInfo1,*pMmsInfo);

			}
		}


		//添加goose sv 
		for (int svgsIndex = 0 ; svgsIndex < pIed->smvGseList.count() ;svgsIndex++ )
		{
			PSMVGSE pSmvGse = pIed->smvGseList[svgsIndex];


			if (  (pSmvGse->control < ATTRIB_SAMPLE)  || (pSmvGse->control > ATTRIB_EQU_PARAM)) //goose
			{

				int  gsIndex = 1;
				GOOSEINFO *pGsInfo = new GOOSEINFO();
				pGsInfo->datCount = 0;
				pIed1->gsList.push_back(pGsInfo);  //添加sv控制块
				pGsInfo->attribId = pSmvGse->control;  //属性id

				//报告控制块
				strcpy(pGsInfo->reportCtrl.ld,pSmvGse->reportCtrl.ld);
				strcpy(pGsInfo->reportCtrl.name,pSmvGse->reportCtrl.name);
				strcpy(pGsInfo->reportCtrl.datSet,pSmvGse->reportCtrl.datSet);
				strcpy(pGsInfo->reportCtrl.rptID,pSmvGse->reportCtrl.rptID);
				pGsInfo->reportCtrl.max = pSmvGse->reportCtrl.max;


				for (int dataSetIndex = 0 ;dataSetIndex < pSmvGse->dataSetList.count() ; dataSetIndex++ )
				{
					PDATASET pDataSet = pSmvGse->dataSetList.at(dataSetIndex);
					sprintf(pGsInfo->datSet,"%s",pDataSet->name.toAscii().data());

					for (int chnIndex= 0 ; chnIndex < pDataSet->fcdaList.count() ; chnIndex++)
					{
						PFCDA pFcda = pDataSet->fcdaList.at(chnIndex);


						if (1)
						{

							t_char   strDefaultVal[MAX_STRLEN]; //默认值

							t_bool bRet = false;
							QList<PCUSTOM_DA> daList;
							QList<QString> strList;
							getDataType(pFcda,daList);
							getDaName(daList,strList);
							freeDaName(daList);

							for (int strListIndex = 0 ; strListIndex < strList.count(); strListIndex++)
							{
								QString strDaAType = strList.at(strListIndex);
								QStringList daATypeList = strDaAType.split("/"); 
								if (daATypeList.count() <2)
									continue;

								QString daName = daATypeList.at(0);
								QString daType = daATypeList.at(1);


								GSCHNINFO *pgseChnInfo = new GSCHNINFO();
								pGsInfo->chnList.push_back(pgseChnInfo);  //添加goose通道信息

								pgseChnInfo->index = gsIndex++;
								strcpy(pgseChnInfo->chnName,pFcda->chnName.toAscii().data());        //通道名称

								pgseChnInfo->dataType = getDataTypeValue(daType.isEmpty()?"SPS":daType,strDefaultVal);
								pgseChnInfo->dataSize = getDataSizeValue(pgseChnInfo->dataType);
								sprintf(pgseChnInfo->dataTypeStr,"%s",daType.isEmpty()?"SPS":daType.toAscii().data());
								strcpy(pgseChnInfo->strVal,strDefaultVal);

								QString datSetName = QString("%1%2/%3%4%5$%6$%7%8")
									.arg(pIed->iedName)
									.arg(pFcda->ldInst)
									.arg(pFcda->prefix)
									.arg(pFcda->lnClass)
									.arg(pFcda->lnInst)
									.arg(pFcda->fc)
									.arg(pFcda->doName)
									.arg(daName.isEmpty() ? "":QString("$%1").arg(daName) );

								datSetName.replace(".","$");
								sprintf(pgseChnInfo->datSetName,"%s",datSetName.toAscii().data());

							}
						}							
					}

				}

				pGsInfo->datCount = pGsInfo->chnList.size();
			}
			else
			{
				SVINFO *pSvInfo = new SVINFO();
				pIed1->svList.push_back(pSvInfo); //添加sv控制块
				pSvInfo->datCount = 0;
				int  svIndex = 1;

				sprintf(pSvInfo->datSet,"%s",pSmvGse->datSet.toAscii().data());  //
				pSvInfo->attribId = pSmvGse->control;  //属性id

				for (int dataSetIndex = 0 ;dataSetIndex < pSmvGse->dataSetList.count() ;dataSetIndex++)
				{
					PDATASET pDataSet = pSmvGse->dataSetList.at(dataSetIndex);

					/* if (pDataSet->name == "dsParameter")
					{
					QString str = "";
					}*/


					for (int chnIndex= 0 ; chnIndex < pDataSet->fcdaList.count() ; chnIndex++)
					{
						PFCDA pFcda = pDataSet->fcdaList.at(chnIndex);


						bRet = false;
						if (1)
						{
							t_char   strDefaultVal[MAX_STRLEN]; //默认值
							t_bool bRet = false;
							QList<PCUSTOM_DA> daList;
							QList<QString> strList;
							getDataType(pFcda,daList);
							getDaName(daList,strList);
							freeDaName(daList);

							for (int strListIndex = 0 ; strListIndex < strList.count(); strListIndex++)
							{
								QString strDaAType = strList.at(strListIndex);
								QStringList daATypeList = strDaAType.split("/"); 
								if (daATypeList.count() <2)
									continue;

								QString daName = daATypeList.at(0);
								QString daType = daATypeList.at(1);

								SVCHNINFO *psvChnInfo = new SVCHNINFO();
								pSvInfo->chnList.push_back(psvChnInfo); //添加通道信息

								psvChnInfo->index  = svIndex++;                            //序号（1~N）
								strcpy(psvChnInfo->chnName,pFcda->chnName.toAscii().data());        //通道名称


								psvChnInfo->maxVal = pFcda->maxVal.toFloat();
								psvChnInfo->minVal = pFcda->minVal.toFloat();
								psvChnInfo->stepVal = pFcda->stepVal.toFloat();
								sprintf(psvChnInfo->unitStr,"%s",pFcda->unitVal.toAscii().data());

								psvChnInfo->dataType = getDataTypeValue(daType,strDefaultVal);
								psvChnInfo->dataSize = getDataSizeValue(psvChnInfo->dataType);
								sprintf(psvChnInfo->dataTypeStr,"%s",daType.toAscii().data());
								if (pSvInfo->attribId == 20)
								{
									QString strVal="";
									QString strSplite="";
									/*for (int tmpGrpIndex = 0 ; tmpGrpIndex < pIed1->settingCtrl.numOfSGs; tmpGrpIndex++)
									{
									strVal += QString("%1%2").arg(strSplite).arg(strDefaultVal);
									strSplite = ",";
									}
									strcpy(psvChnInfo->strVal,strVal.toAscii().data());*/
								}
								else
								{
									strcpy(psvChnInfo->strVal,strDefaultVal);
								}


								QString datSetName = QString("%1%2/%3%4%5$%6$%7%8")
									.arg(pIed->iedName)
									.arg(pFcda->ldInst)
									.arg(pFcda->prefix)
									.arg(pFcda->lnClass)
									.arg(pFcda->lnInst)
									.arg(pFcda->fc)
									.arg(pFcda->doName)
									.arg(daName.isEmpty() ? "":QString("$%1").arg(daName) );

								datSetName.replace(".","$");

								strcpy(psvChnInfo->datSetName,datSetName.toAscii().data());                 //数据对象名


							}	
						}
					}

				}

				pSvInfo->datCount= pSvInfo->chnList.size();
			}
		}
	}



}



void ClientDSE_ScdManagerImpl::getDaName(QList<PCUSTOM_DA> &daList,QList<QString> &strList)
{
	if (daList.count() <= 0 )
		return ;


	for (int index = 0 ; index < daList.count() ; index++)
	{
		PCUSTOM_DA pCusDa = daList.at(index);

		if (pCusDa->custDaList.count() <= 0)
		{
			QString daName = QString("%1/%2").arg(pCusDa->daName).arg(pCusDa->daDataType);
			strList.append(daName);
		}
		else
		{
			getDaName(pCusDa->custDaList,strList);

		}
	}

}

void   ClientDSE_ScdManagerImpl::freeDaName(QList<PCUSTOM_DA> &daList)
{
	if (daList.count() <= 0 )
		return ;

	for (int index = 0 ; index < daList.count() ; index++)
	{
		PCUSTOM_DA pCusDa = daList.at(index);

		if (pCusDa->custDaList.count() >0)
		{
			freeDaName(pCusDa->custDaList);
		}

		delete pCusDa;
	}

}

void ClientDSE_ScdManagerImpl::makeScdInf1()
{

	QStringList strList;
	t_uint8     strCount = 0;
	t_bool     bRet = false;
	//生成一个子网络节点
	PSUBNET pSubNet = new SUBNET();
	m_subNetList->push_back(pSubNet);

	//进度指示
	int  tmpFreeSpace = m_fileProgress.fileTotalLen-m_fileProgress.fileLengthed;
	int  tmpStep = 0;
	if (tmpFreeSpace>0)
	{
		tmpStep = tmpFreeSpace/(m_equipTotal+1);  //1是多增加一个装置，解析完再显示成100%
	}

	if (m_subSta)
	{
		sprintf(m_subSta->name,"%s",m_sta.name.toAscii().data());  //子站名称
	}


	for (int iedIndex = 0 ;iedIndex < m_sta.iedList.count() ;iedIndex++)
	{
		PINTERNAL_IED1 pIed = m_sta.iedList[iedIndex];

		PIED pIed1 = new IED();
		pSubNet->iedList.push_back(pIed1);  //创建装置


		strcpy( pIed1->name,pIed->iedName.toAscii().data());
		strcpy( pIed1->desc,pIed->desc.toAscii().data());
		strcpy(pIed1->type,pIed->apName.toAscii().data());
		//为模拟服务器增加定值控制块
		pIed1->settingCtrl.actSG     = m_settingCtrl.actSG;
		pIed1->settingCtrl.numOfSGs  = m_settingCtrl.numOfSGs;


		//进度指示
		m_fileProgress.fileLengthed += tmpStep;

		CMD_TYPE_DEF::CMD cmd;

		cmd.head.cmd = CMD_TYPE_DEF::CMD_FILE_PROGRESSS;
		memcpy(cmd.data.str,&m_fileProgress,sizeof(CMD_TYPE_DEF::FILE_PROGRESS));
		//回调，传送的文件已经接收的文件长度
		if (m_param)
			m_routine(m_param,cmd);



		//添加mms节点
		PINTERNAL_IED pComLayerIed = getCommLayerIed(pIed1->name);

		if (pComLayerIed)
		{
			for (int mmsIndex = 0 ; mmsIndex < pComLayerIed->mmsList.size() ; mmsIndex++)
			{
				MMSINFO *pMmsInfo = pComLayerIed->mmsList[mmsIndex];

				MMSINFO *pMmsInfo1 = new MMSINFO();
				pIed1->mmsList.push_back(pMmsInfo1);

				copyFrom(*pMmsInfo1,*pMmsInfo);

			}
		}


		//添加goose sv 
		for (int svgsIndex = 0 ; svgsIndex < pIed->smvGseList.count() ;svgsIndex++ )
		{
			PSMVGSE pSmvGse = pIed->smvGseList[svgsIndex];


			if (  (pSmvGse->control < ATTRIB_SAMPLE)  || (pSmvGse->control > ATTRIB_EQU_PARAM)) //goose
			{

				GOOSEINFO *pGsInfo = new GOOSEINFO();
				pGsInfo->datCount = 0;
				pIed1->gsList.push_back(pGsInfo);  //添加sv控制块
				pGsInfo->attribId = pSmvGse->control;  //属性id

				//报告控制块
				strcpy(pGsInfo->reportCtrl.ld,pSmvGse->reportCtrl.ld);
				strcpy(pGsInfo->reportCtrl.name,pSmvGse->reportCtrl.name);
				strcpy(pGsInfo->reportCtrl.datSet,pSmvGse->reportCtrl.datSet);
				strcpy(pGsInfo->reportCtrl.rptID,pSmvGse->reportCtrl.rptID);
				pGsInfo->reportCtrl.max = pSmvGse->reportCtrl.max;


				for (int dataSetIndex = 0 ;dataSetIndex < pSmvGse->dataSetList.count() ; dataSetIndex++ )
				{
					PDATASET pDataSet = pSmvGse->dataSetList.at(dataSetIndex);
					sprintf(pGsInfo->datSet,"%s",pDataSet->name.toAscii().data());

					for (int chnIndex= 0 ; chnIndex < pDataSet->fcdaList.count() ; chnIndex++)
					{
						PFCDA pFcda = pDataSet->fcdaList.at(chnIndex);

						GSCHNINFO *pgseChnInfo = new GSCHNINFO();
						pGsInfo->chnList.push_back(pgseChnInfo);  //添加goose通道信息

						pgseChnInfo->index = chnIndex+1;
						strcpy(pgseChnInfo->chnName,pFcda->chnName.toAscii().data());        //通道名称


						if (1)
						{

							t_char   strDefaultVal[MAX_STRLEN]; //默认值

							t_bool bRet = false;
							QString strDataType =  getDataType(pFcda->lnType,pFcda->lnClass,pFcda->lnInst,pFcda->doName,pFcda->fc,pFcda->daName,bRet);
							pgseChnInfo->dataType = getDataTypeValue(strDataType.isEmpty()?"SPS":strDataType,strDefaultVal);
							pgseChnInfo->dataSize = getDataSizeValue(pgseChnInfo->dataType);
							sprintf(pgseChnInfo->dataTypeStr,"%s",strDataType.isEmpty()?"SPS":strDataType.toAscii().data());
							strcpy(pgseChnInfo->strVal,strDefaultVal);
						}

						QString datSetName = QString("%1%2/%3%4%5$%6$%7%8")
							.arg(pIed->iedName)
							.arg(pFcda->ldInst)
							.arg(pFcda->prefix)
							.arg(pFcda->lnClass)
							.arg(pFcda->lnInst)
							.arg(pFcda->fc)
							.arg(pFcda->doName)
							.arg(pFcda->daName.isEmpty() ? "":QString("$%1").arg(pFcda->daName) );

						datSetName.replace(".","$");
						sprintf(pgseChnInfo->datSetName,"%s",datSetName.toAscii().data());

					}

					pGsInfo->datCount += pDataSet->fcdaList.count();
				}
			}
			else
			{
				SVINFO *pSvInfo = new SVINFO();
				pIed1->svList.push_back(pSvInfo); //添加sv控制块
				pSvInfo->datCount = 0;

				sprintf(pSvInfo->datSet,"%s",pSmvGse->datSet.toAscii().data());  //
				pSvInfo->attribId = pSmvGse->control;  //属性id

				for (int dataSetIndex = 0 ;dataSetIndex < pSmvGse->dataSetList.count() ;dataSetIndex++)
				{
					PDATASET pDataSet = pSmvGse->dataSetList.at(dataSetIndex);

					for (int chnIndex= 0 ; chnIndex < pDataSet->fcdaList.count() ; chnIndex++)
					{
						PFCDA pFcda = pDataSet->fcdaList.at(chnIndex);

						SVCHNINFO *psvChnInfo = new SVCHNINFO();
						pSvInfo->chnList.push_back(psvChnInfo); //添加通道信息

						psvChnInfo->index  = chnIndex+1;                            //序号（1~N）
						strcpy(psvChnInfo->chnName,pFcda->chnName.toAscii().data());        //通道名称


						psvChnInfo->maxVal = pFcda->maxVal.toFloat();
						psvChnInfo->minVal = pFcda->minVal.toFloat();
						psvChnInfo->stepVal = pFcda->stepVal.toFloat();
						sprintf(psvChnInfo->unitStr,"%s",pFcda->unitVal.toAscii().data());

						strList = pFcda->daName.split(".");
						strCount = strList.count();

						bRet = false;
						if (1)
						{
							if (strList.count() >=1)
							{
								QString f = strList.at(strCount-1);
								if (f == "f")
								{
									psvChnInfo->dataType = _GSDATA_TYPE::GVE_FLOAT32;
									psvChnInfo->dataSize  =4;
									sprintf(psvChnInfo->dataTypeStr,"FLOAT32");
									bRet = true;
								}
							}

							if (bRet == false)
							{

								t_char   strDefaultVal[MAX_STRLEN]; //默认值
								bRet = false;
								QString strDataType =  getDataType(pFcda->lnType,pFcda->lnClass,pFcda->lnInst,pFcda->doName,pFcda->fc,pFcda->daName,bRet);
								psvChnInfo->dataType = getDataTypeValue(strDataType,strDefaultVal);
								psvChnInfo->dataSize = getDataSizeValue(psvChnInfo->dataType);
								sprintf(psvChnInfo->dataTypeStr,"%s",strDataType.toAscii().data());
								strcpy(psvChnInfo->strVal,strDefaultVal);
							}

						}



						QString datSetName = QString("%1%2/%3%4%5$%6$%7%8")
							.arg(pIed->iedName)
							.arg(pFcda->ldInst)
							.arg(pFcda->prefix)
							.arg(pFcda->lnClass)
							.arg(pFcda->lnInst)
							.arg(pFcda->fc)
							.arg(pFcda->doName)
							.arg(pFcda->daName.isEmpty() ? "":QString("$%1").arg(pFcda->daName) );

						datSetName.replace(".","$");

						strcpy(psvChnInfo->datSetName,datSetName.toAscii().data());                 //数据对象名

					}

					pSvInfo->datCount += pDataSet->fcdaList.count();

				}

			}		 
		}
	}

}



PSMVGSE  ClientDSE_ScdManagerImpl::getSmvgse(PINTERNAL_IED1 pIEd1,QString cbName,QString ldInst)
{

	for (int i = 0 ;i < pIEd1->smvGseList.size(); i++)
	{
		PSMVGSE psmvGse = pIEd1->smvGseList[i];

		if ( (psmvGse->cbName == cbName) && (psmvGse->inst == ldInst) )
			return psmvGse;
	}

	return NULL;


}


t_int8   ClientDSE_ScdManagerImpl::getContrlAttrib(QString dataSetName,t_int8 &attribId)
{

	if (dataSetName.contains(DS_SETTING))        //定值
	{

		attribId = ATTRIB_SETTING;

		return 0;
	}
	else if (dataSetName.contains(DS_EQU_PARAM))  //装置参数
	{
		attribId = ATTRIB_EQU_PARAM;

		return 0;
	}
	else if ( dataSetName.contains(DS_RELAY_AIN) || dataSetName.contains(DS_AIN) || dataSetName.contains(DS_URCB_RELAY_AIN) || dataSetName.contains(DS_UBRCB_AIN)  )   //遥测值
	{
		attribId = ATTRIB_SAMPLE;

		return 0;
	}
	else if (dataSetName.contains(DS_RELAY_DIN) || dataSetName.contains(DS_DIN) || dataSetName.contains(DS_BRCB_DIN) || dataSetName.contains(DS_BRCB_RELAY_DIN))  //遥信值
	{
		attribId = ATTRIB_SWI;

		return 0;
	}
	else if (dataSetName.contains(DS_RELAY_ENA) || dataSetName.contains(DS_BRCB_REALY_ENA) || dataSetName.contains(DS_ENA))   //压板，这里不能区分是软压板还是硬压板
	{
		attribId = ATTRIB_YB;

		return 0;
	}
	else if (dataSetName.contains(DS_WARNING) || dataSetName.contains(DS_ALARM) || dataSetName.contains(DS_BRCB_ALARM) || dataSetName.contains(DS_BRCB_WARNING))//告警
	{
		attribId = ATTRIB_ALARM;

		return 0;
	}
	else if (dataSetName.contains(DS_TRIP_INFO) || dataSetName.contains(DS_BRCB_TRIP_INFO))   //事件
	{
		attribId = ATTRIB_EVENT;

		return 0;
	}
	else if (dataSetName.contains(DS_RELAYREC) || dataSetName.contains(DS_BRCB_REALY_RECV))   //录波简报
	{
		attribId = ATTRIB_RELAYREC;

		return 0;
	}




	return -1;
}



ClientDSE_ScdManager::ClientDSE_ScdManager()
{
}

ClientDSE_ScdManager::~ClientDSE_ScdManager()
{
	clear();
}



void ClientDSE_ScdManager::copyFrom( ClientDSE_ScdManager &from)
{
	clear();


	for (t_uint i = 0 ;i < from.m_subNetList.size() ;i++)
	{
		PSUBNET pSubNet = from.m_subNetList[i];

		PSUBNET pSubNet1 = new SUBNET();
		m_subNetList.push_back(pSubNet1);
		copyFrom(*pSubNet1,*pSubNet);


	}

}


t_int  ClientDSE_ScdManager::readScd(const t_char *fileName,	
									 CMD_TYPE_DEF::LPRECVROUTINE  callbackfun,			//接收消息的回调函数。
									 void*                        callbackfunparam,	//回调函数参数。
									 t_uint8  flag,
									 t_char *err)
{

	ClientDSE_ScdManagerImpl impl;

	clear();

	//设置进度显示的回调函数
	impl.set_routine(callbackfun,callbackfunparam);
	t_int ret=  impl.readScd(fileName,m_subNetList,m_subSta,flag,err);


	return ret;


}




t_int  ClientDSE_ScdManager::read(const t_char *fileName,char *err)
{

	ClientDSE_ScdManagerImpl impl;

	clear();
	return impl.read(fileName,m_subNetList,m_subSta,err);

}

t_int ClientDSE_ScdManager::read_event(const t_char *fileName,t_int attribId,t_bool isSv,t_char *iedName,SETTING_ITEM1 *pEvent,t_uint32 *event_nums,t_uint32 max_nums,QHash<QString, int> &hash,t_char *err)
{
	t_uint32 tmp_event_nums = 0;


	for (int netIndex = 0 ; netIndex < m_subNetList.size() ; netIndex++)
	{
		PSUBNET pSubNet = m_subNetList[netIndex];

		for (int  iedIndex = 0 ; iedIndex < pSubNet->iedList.size() ; iedIndex++)
		{
			PIED pIed = pSubNet->iedList[iedIndex];

			if (strcmp(pIed->name,iedName) != 0)
				continue;

			if (isSv)
			{
				for (int svIndex = 0 ; svIndex < pIed->svList.size() ; svIndex++)
				{
					SVINFO *pSvInfo = pIed->svList[svIndex];

					if (pSvInfo->attribId  != attribId)
						continue;

					hash[QString("%1").arg(pSvInfo->datSet)] = 1;

					for (int svChnIndex = 0 ; svChnIndex < pSvInfo->chnList.size() ; svChnIndex++)
					{
						SVCHNINFO *pSvChnInfo = pSvInfo->chnList[svChnIndex];

						if (tmp_event_nums >=max_nums )
							break;

						strcpy(pEvent[tmp_event_nums].ref,pSvChnInfo->datSetName);
						strcpy(pEvent[tmp_event_nums].desc,pSvChnInfo->chnName);
						pEvent[tmp_event_nums].data_type = pSvChnInfo->dataType;
						pEvent[tmp_event_nums].data_size = pSvChnInfo->dataSize;
						sprintf(pEvent[tmp_event_nums].max_val_str,"%.3f", pSvChnInfo->maxVal);
						sprintf(pEvent[tmp_event_nums].min_val_str,"%.3f", pSvChnInfo->minVal);
						sprintf(pEvent[tmp_event_nums].step_str,"%.3f", pSvChnInfo->stepVal);
						strcpy(pEvent[tmp_event_nums].unit,pSvChnInfo->unitStr);

						tmp_event_nums++;

					}

				}
			}
			else
			{
				for (int gsIndex = 0 ; gsIndex < pIed->gsList.size() ; gsIndex++)
				{
					GOOSEINFO *pGsInfo = pIed->gsList[gsIndex];

					if (pGsInfo->attribId  != attribId)
						continue;

					hash[QString("%1").arg(pGsInfo->datSet)] = 1;

					for (int gsChnIndex = 0 ; gsChnIndex < pGsInfo->chnList.size() ; gsChnIndex++)
					{
						GSCHNINFO *pGsChnInfo = pGsInfo->chnList[gsChnIndex];

						if (tmp_event_nums >=max_nums )
							break;

						strcpy(pEvent[tmp_event_nums].ref,pGsChnInfo->datSetName);
						strcpy(pEvent[tmp_event_nums].desc,pGsChnInfo->chnName);
						pEvent[tmp_event_nums].data_type = pGsChnInfo->dataType;
						pEvent[tmp_event_nums].data_size = pGsChnInfo->dataSize;

						tmp_event_nums++;


					}
				}

			}	

			*event_nums  = tmp_event_nums;
			return 0;
		}
	}

	*event_nums  = tmp_event_nums;

	return 0;
}

t_int  ClientDSE_ScdManager::write(const t_char *fileName,char *err)
{
	ClientDSE_ScdManagerImpl impl;

	t_int ret =  impl.write(fileName,m_subNetList,m_subSta,err);

	return ret;
}


void ClientDSE_ScdManager::clear()
{

	clear(m_subNetList);

}


void ClientDSE_ScdManager::clear(std::vector<PSUBNET> &subNetList)
{
	for (t_uint i = 0 ;i < subNetList.size() ;i++)
	{
		PSUBNET pSubNet = subNetList[i];

		delete pSubNet;
	}

	subNetList.clear();
}


void ClientDSE_ScdManager::copyFrom(SUBNET &dest,SUBNET &src)
{

	strcpy(dest.type,  src.type);
	strcpy(dest.name,  src.name);
	strcpy(dest.desc,  src.desc);

	for (t_uint i = 0;i<src.iedList.size() ;i++)
	{
		PIED pIed = src.iedList[i];

		PIED pIed1 = new IED();
		dest.iedList.push_back(pIed1);
		copyFrom(*pIed1,*pIed);
	}

}

void ClientDSE_ScdManager::copyFrom(GOOSEINFO &dest,GOOSEINFO &src)
{

	dest.customId = src.customId;
	dest.ethIndex = src.ethIndex;
	dest.appid = src.appid;
	strcpy(dest.prefix,  src.prefix);
	strcpy(dest.mac,  src.mac);
	dest.confRev = src.confRev;
	strcpy(dest.gcRef,  src.gcRef);
	strcpy(dest.datSet,  src.datSet);
	strcpy(dest.goId,  src.goId);
	dest.datCount = src.datCount;
	dest.vlan = src.vlan;
	dest.t0 = src.t0;
	dest.t1 = src.t1;
	dest.enable = src.enable;
	dest.attribId = src.attribId;

	for (t_uint i = 0 ;i < src.chnList.size() ;i++)
	{
		PGSCHNINFO pGsChnInfo = src.chnList[i];

		PGSCHNINFO pGsChnInfo1 = new GSCHNINFO();
		dest.chnList.push_back(pGsChnInfo1);
		copyFrom(*pGsChnInfo1,*pGsChnInfo);
	}
}

void ClientDSE_ScdManager::copyFrom(GSCHNINFO &dest,GSCHNINFO &src)
{

	dest.index = src.index;
	strcpy(dest.chnName,  src.chnName);
	strcpy(dest.datSetName,  src.datSetName);

	dest.dataType = src.dataType;


	dest.type = src.type;
	dest.flag = src.flag;
	dest.enable = src.enable;

}

void ClientDSE_ScdManager::copyFrom(MMSINFO &dest,MMSINFO &src)
{

	strcpy(dest.ip,src.ip);
	strcpy(dest.mask,src.mask);
	strcpy(dest.destIp,src.destIp);
	strcpy(dest.destMask,src.destMask);
	strcpy(dest.apName,src.apName);
	dest.appid = src.appid;
	dest.ethIndex = src.ethIndex;
	dest.overtime = src.overtime;

}

void ClientDSE_ScdManager::copyFrom(PTPINFO &dest,PTPINFO &src)
{

	strcpy(dest.name,src.name);
	strcpy(dest.desc,src.desc);
	dest.appid = src.appid;
	dest.ethIndex = src.ethIndex;


}


void ClientDSE_ScdManager::copyFrom(SVCHNINFO &dest,SVCHNINFO &src)
{
	dest.index = src.index;
	dest.remoteIndex = src.remoteIndex;
	strcpy(dest.chnName,  src.chnName);
	strcpy(dest.datSetName,  src.datSetName);
	dest.type = src.type;
	dest.priRated = src.priRated;
	dest.secRated = src.secRated;
	dest.priReferVal = src.priReferVal;
	dest.referToSamp = src.referToSamp;
	dest.overLaod = src.overLaod;
	dest.sampLimit = src.sampLimit;
	dest.twoADDiff = src.twoADDiff;
	dest.flag = src.flag;
	dest.freq = src.freq;
	dest.range = src.range;
	dest.blm = src.blm;
	dest.smpMin = src.smpMin;
	dest.enable = src.enable;
	dest.precipValue = src.precipValue;
	dest.maxVal = src.maxVal;                             //最大值
	dest.minVal = src.minVal;                             //最小值
	dest.stepVal =src.stepVal;                            //步长
	strcpy(dest.unitStr,src.unitStr); //单位

}

void ClientDSE_ScdManager::copyFrom(SVINFO &dest,SVINFO &src)
{

	dest.customId = src.customId;
	dest.ethIndex = src.ethIndex;
	dest.appid = src.appid;
	strcpy(dest.prefix,  src.prefix);
	strcpy(dest.mac,  src.mac);
	dest.confRev = src.confRev;
	strcpy(dest.svId,  src.svId);
	strcpy(dest.datSet,  src.datSet);
	strcpy(dest.ldDevName,  src.ldDevName);
	strcpy(dest.lnName,  src.lnName);
	dest.datCount = src.datCount;
	dest.datSetType = src.datSetType;
	dest.convertType = src.convertType;
	dest.vlan = src.vlan;
	dest.enable = src.enable;


	dest.curInfo.priRated = src.curInfo.priRated;
	dest.curInfo.secRated = src.curInfo.secRated;
	dest.curInfo.priReferVal = src.curInfo.priReferVal;
	dest.curInfo.referToSamp = src.curInfo.referToSamp;
	dest.curInfo.overLaod = src.curInfo.overLaod;
	dest.curInfo.sampLimit = src.curInfo.sampLimit;
	dest.curInfo.twoADDiff = src.curInfo.twoADDiff;
	dest.curInfo.precipValue = src.curInfo.precipValue;

	dest.volInfo.priRated    = src.volInfo.priRated;
	dest.volInfo.secRated    = src.volInfo.secRated;
	dest.volInfo.priReferVal = src.volInfo.priReferVal;
	dest.volInfo.referToSamp = src.volInfo.referToSamp;
	dest.volInfo.overLaod    = src.volInfo.overLaod;
	dest.volInfo.sampLimit   = src.volInfo.sampLimit;
	dest.volInfo.twoADDiff   = src.volInfo.twoADDiff;
	dest.volInfo.precipValue = src.volInfo.precipValue;

	for (t_uint i = 0 ; i< src.chnList.size() ;i++)
	{
		PSVCHNINFO pSvChnInfo = src.chnList[i];

		PSVCHNINFO pSvChnInfo1 = new SVCHNINFO();
		dest.chnList.push_back(pSvChnInfo1);
		copyFrom(*pSvChnInfo1,*pSvChnInfo);
	}

}

void ClientDSE_ScdManager::copyFrom(IED &dest,IED &src)
{

	strcpy(dest.type,  src.type);
	strcpy(dest.name,  src.name);
	strcpy(dest.desc,  src.desc);
	strcpy(dest.manufacturer,  src.manufacturer);
	dest.settingCtrl.actSG    = src.settingCtrl.actSG;
	dest.settingCtrl.numOfSGs = src.settingCtrl.numOfSGs;

	for (t_uint i = 0 ; i < src.gsList.size();i++)
	{
		PGOOSEINFO pGooseInfo = src.gsList[i];

		PGOOSEINFO pGooseInfo1 = new GOOSEINFO();
		dest.gsList.push_back(pGooseInfo1);
		copyFrom(*pGooseInfo1,*pGooseInfo);
	}
	for (t_uint i = 0 ; i < src.svList.size();i++)
	{
		PSVINFO pSvInfo = src.svList[i];

		PSVINFO pSvInfo1 = new SVINFO();
		dest.svList.push_back(pSvInfo1);
		copyFrom(*pSvInfo1,*pSvInfo);

	}

	for (t_uint i = 0 ;i < src.mmsList.size() ;i++)
	{
		PMMSINFO  pMms =  src.mmsList[i];

		PMMSINFO pMms1 = new MMSINFO();
		dest.mmsList.push_back(pMms1);
		copyFrom(*pMms1,*pMms);

	}

	for (t_uint i = 0 ;i < src.ptpList.size() ;i++)
	{
		PPTPINFO  pPtp =  src.ptpList[i];

		PPTPINFO pPtp1 = new PTPINFO();
		dest.ptpList.push_back(pPtp1);
		copyFrom(*pPtp1,*pPtp);

	}


}


t_int  ClientDSE_ScdManager::getIedAndApName(t_char *iedName,t_char *apName)
{

	for (t_int netIndex = 0 ; netIndex < m_subNetList.size() ;netIndex++)
	{
		PSUBNET pSubNet = m_subNetList[netIndex];

		for (t_int iedIndex = 0 ; iedIndex < pSubNet->iedList.size() ; iedIndex++)
		{
			PIED pIed = pSubNet->iedList[iedIndex];

			sprintf(iedName,"%s",pIed->name);

			for (t_int mmsIndex = 0 ; mmsIndex < pIed->mmsList.size() ; mmsIndex++)
			{
				MMSINFO *pMms = pIed->mmsList[mmsIndex];

				sprintf(apName,"%s",pMms->apName);
				return 0;
			}
		}
	}

	return -1;

}

t_bool    ClientDSE_ScdManager::readConnParam(QString &driver,QString &host,QString &database,QString &user,QString &pwd)
{

	QDomDocument		domDocument;
	QDomNodeList albums;

	QString errorStr;
	int errorLine;
	int errorColumn;

	QString strDir = QDir::currentPath();


	QFile file(strDir+"/config/config.xml");
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
		return false;

	if (!domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn)) 
		return false;

	QDomElement root = domDocument.documentElement();
	if (root.tagName() != "Param")
		return false;

	albums = domDocument.elementsByTagName("Driver");
	if( albums.count() > 0)
		driver = albums.item(0).toElement().text();

	albums = domDocument.elementsByTagName("DBServer");
	if( albums.count() > 0)
		host = albums.item(0).toElement().text();

	albums = domDocument.elementsByTagName("DataBase");
	if( albums.count() > 0)
		database = albums.item(0).toElement().text();

	albums = domDocument.elementsByTagName("UserName");
	if( albums.count() > 0)
		user = albums.item(0).toElement().text();

	albums = domDocument.elementsByTagName("Password");
	if( albums.count() > 0)
		pwd = albums.item(0).toElement().text();


	return true;


}


t_bool   ClientDSE_ScdManager::readConnParam1(QString &driver,QString &host,QString &database,QString &user,QString &pwd)
{

	QString strPath = QCoreApplication::applicationDirPath ();
	QString strfileName = strPath + QString("/") + "nrnetwork.ini";

	QSettings settings(strfileName, QSettings::IniFormat);

	driver     =  settings.value ("DATABASE/DIRVER",   "QMYSQL").toString (); 
	host       = settings.value ("DATABASE/HOSTNAME",   "127.0.0.1").toString (); 
	database   = settings.value ("DATABASE/DATABASENAME",   "protected").toString (); 
	user       = settings.value ("DATABASE/USER",   "fh5000").toString (); 
	pwd        = settings.value ("DATABASE/PASSWD",   "fh5000").toString (); 

	return true;

}

t_bool  ClientDSE_ScdManager::connDb(QString driver,QString host,QString database,QString user,QString pwd)
{
	static int s_temp = 0;
	QString connName = QString("fh3k_conn_%1").arg(s_temp++);
	if(QSqlDatabase::contains(connName))
	{
		if(QSqlDatabase::database(connName).isOpen())
		{
			return true;
		}
		else
		{
			QSqlDatabase::removeDatabase(connName);
			return false;
		}
	}
	else
	{
		QSqlDatabase::addDatabase(driver, connName);
		QSqlDatabase::database(connName).setHostName(host);
		QSqlDatabase::database(connName).setDatabaseName(database);
		QSqlDatabase::database(connName).setUserName(user);
		QSqlDatabase::database(connName).setPassword(pwd);
	}


	t_bool bRet = true;
	if(!QSqlDatabase::database(connName).open())
	{
		QSqlError error = QSqlDatabase::database(connName).lastError();
		qDebug() << ("打开数据库失败 err:%s\n",error.text().toAscii().data());
		bRet = false;
	}

	if (bRet)
	{
		//printf("打开数据库成功 connName:%s\n",connName.toAscii().data());
	}
	else
	{
		QSqlDatabase::removeDatabase(connName);
		printf("打开数据库失败 connName:%s\n",connName.toAscii().data());
	}

	m_connName = connName;


	return bRet;


}

void  ClientDSE_ScdManager::closeDb()
{

	QSqlDatabase::removeDatabase(m_connName);
}

void   ClientDSE_ScdManager:: deleteTable()
{
	QStringList strAttribIdList ;

	strAttribIdList << QString("%1").arg(ATTRIB_SETTING)
		<< QString("%1").arg(ATTRIB_EQU_PARAM)
		<< QString("%1").arg(ATTRIB_SAMPLE)
		<< QString("%1").arg(ATTRIB_SWI)
		<< QString("%1").arg(ATTRIB_YB)
		<< QString("%1").arg(ATTRIB_ALARM)
		<< QString("%1").arg(ATTRIB_EVENT)
		<< QString("%1").arg(ATTRIB_REPORTCTRL)
		<< QString("%1").arg(ATTRIB_RELAYREC);

	QSqlQuery query("",QSqlDatabase::database(m_connName));

	for (int i = 0 ; i < strAttribIdList.count() ; i++)
	{

		QString str = strAttribIdList.at(i);
		t_uint8 attribId = strAttribIdList.at(i).toInt();

		QString sql	= QString("DELETE FROM %1 WHERE InfAttribId=%2")
			.arg( m_tableName)
			.arg( attribId);

		if (!query.exec(sql))
		{
			QString errStr = QString("%1,sql:%2").arg(query.lastError().text()).arg(sql);
			printf("%s\n",errStr.toAscii().data());
			return ;
		}
	}
}

t_bool  ClientDSE_ScdManager::insertItem1(t_uint8 CpuId,
										  t_uint8 InfFun,t_uint8 InfInf,
										  t_uint8 InfEnabled,t_uint8 InfDataType,
										  t_uint8 InfAttribId,QString InfName,
										  QString InfCode,QString InfUnit,
										  t_uint8 InfValueType,t_float InfMaxValue,
										  t_float InfMinValue,t_float InfStep,
										  QString InfDefaultValue,t_float InfFactor,
										  QString InfPrecision,t_int InfKod,
										  t_int InfType,t_int InfSize,
										  t_int InfNumber,t_int InfDisplevelAutoSend,
										  QString InfReference,t_int AssociateInfFun ,
										  QString Iec61850EntryID 
										  )
{

	QSqlQuery query("",QSqlDatabase::database(m_connName));

	QString fields   = QString( " `CpuId`,`InfFun`,`InfInf`,`InfEnabled`,`InfDataType`,`InfAttribId`,`InfName`,`InfCode`,`InfUnit`,`InfValueType`,`InfMaxValue`,`InfMinValue`,`InfStep`,`InfDefaultValue`,`InfFactor`,`InfPrecision`,`InfKod`,`InfType`,`InfSize`,`InfNumber`,`InfDisplevelAutoSend`,`InfReference`,`AssociateInfFun`,`Iec61850EntryID`");

	QString values	 =  QString( " %1,%2,%3,%4,%5,%6,'%7','%8','%9',%10,%11,%12,%13,'%14',%15,'%16',%17,%18,%19,%20,%21,'%22',%23,'%24'")
		.arg(CpuId)
		.arg(InfFun)
		.arg(InfInf)
		.arg(InfEnabled)
		.arg(InfDataType)
		.arg(InfAttribId)
		.arg(InfName)
		.arg(InfCode)
		.arg(InfUnit)
		.arg(InfValueType)
		.arg(InfMaxValue)
		.arg(InfMinValue)
		.arg(InfStep)
		.arg(InfDefaultValue)
		.arg(InfFactor)
		.arg(InfPrecision)
		.arg(InfKod)
		.arg(InfType)
		.arg(InfSize)
		.arg(InfNumber)
		.arg(InfDisplevelAutoSend)
		.arg(InfReference)
		.arg(AssociateInfFun)
		.arg(Iec61850EntryID);


	QString sql		=	QString("INSERT %1(%2) VALUES(%3)")
		.arg( m_tableName)
		.arg( fields)
		.arg( values);


	if (!query.exec(sql))
	{
		QString errStr = QString("%1,sql:%2").arg(query.lastError().text()).arg(sql);
		printf("%s\n",errStr.toAscii().data());
		return false;
	}

	return true;

}

t_bool  ClientDSE_ScdManager::insertItem(t_uint8 CpuId,
										 t_uint8 InfFun,t_uint8 InfInf,
										 t_uint8 InfGroupId,t_uint8 InfItemId,
										 t_uint8 InfEnabled,t_uint8 InfDataType,
										 t_uint8 InfAttribId,QString InfName,
										 QString InfCode,QString InfUnit,
										 t_uint8 InfValueType,t_float InfMaxValue,
										 t_float InfMinValue,t_float InfStep,
										 QString InfDefaultValue,t_float InfFactor,
										 QString InfPrecision,t_int InfKod,
										 t_int InfType,t_int InfSize,
										 t_int InfNumber,t_int InfDisplevelAutoSend,
										 QString InfReference,t_int AssociateInfFun ,
										 QString Iec61850EntryID ,QString Iec61850RptID
										 )
{

	QSqlQuery query("",QSqlDatabase::database(m_connName));

	QString fields   = QString( " `CpuId`,`InfFun`,`InfInf`,`InfGroupId`,`InfItemId`,`InfEnabled`,`InfDataType`,`InfAttribId`,`InfName`,`InfCode`,`InfUnit`,`InfValueType`,`InfMaxValue`,`InfMinValue`,`InfStep`,`InfDefaultValue`,`InfFactor`,`InfPrecision`,`InfKod`,`InfType`,`InfSize`,`InfNumber`,`InfDisplevelAutoSend`,`InfReference`,`AssociateInfFun`,`Iec61850EntryID`,`Iec61850RptID` ");

	QString values	 =  QString( " %1,%2,%3,%4,%5,%6,%7,%8,'%9','%10','%11',%12,%13,%14,%15,'%16',%17,'%18',%19,%20,%21,%22,%23,'%24',%25,'%26','%27'")
		.arg(CpuId)
		.arg(InfFun)
		.arg(InfInf)
		.arg(InfGroupId)
		.arg(InfItemId)
		.arg(InfEnabled)
		.arg(InfDataType)
		.arg(InfAttribId)
		.arg(InfName)
		.arg(InfCode)
		.arg(InfUnit)
		.arg(InfValueType)
		.arg(InfMaxValue)
		.arg(InfMinValue)
		.arg(InfStep)
		.arg(InfDefaultValue)
		.arg(InfFactor)
		.arg(InfPrecision)
		.arg(InfKod)
		.arg(InfType)
		.arg(InfSize)
		.arg(InfNumber)
		.arg(InfDisplevelAutoSend)
		.arg(InfReference)
		.arg(AssociateInfFun)
		.arg(Iec61850EntryID)
		.arg(Iec61850RptID);

	QString sql		=	QString("INSERT %1(%2) VALUES(%3)")
		.arg( m_tableName)
		.arg( fields)
		.arg( values);


	if (!query.exec(sql))
	{
		QString errStr = QString("%1,sql:%2").arg(query.lastError().text()).arg(sql);
		printf("%s\n",errStr.toAscii().data());
		return false;
	}

	return true;
}


/*
InfName  InfAttribId  InfValueType
开关量     0         4   
软压板     1         4   
模拟量     10        0   
定值       20        0   
装置参数   23        0
告警       30        10
事件       40        9
报告       160       3
*/
t_uint8 ClientDSE_ScdManager::getInfValueType(t_int attribId)
{
	t_uint8 valueType = 0;

	switch(attribId)
	{
	case ATTRIB_SETTING:
	case ATTRIB_EQU_PARAM:
	case ATTIRB_RUN_SETTING_NONE:
	case ATTIRB_CUR_SETTING_NONE:
	case ATTRIB_SAMPLE:
		valueType = 0;
		break;
	case ATTRIB_SWI:
	case ATTRIB_YB:
	case ATTRIB_YYB:
		valueType = 4;
		break;
	case ATTRIB_ALARM:
	case ATTRIB_SELF_CHECK:
		valueType = 10;
		break;
	case ATTRIB_EVENT:
		valueType = 9;
		break;
	case ATTRIB_REPORTCTRL:
		valueType = 3;
		break;
	}

	return valueType;

}


QString ClientDSE_ScdManager::getInfPrecision(t_int dataType)
{
	QString strPrec = "";

	switch(dataType)
	{
	case _GSDATA_TYPE::GV_NONE:
		strPrec = "0x30";
		break;
	case _GSDATA_TYPE::GVE_INT8U:
	case _GSDATA_TYPE::GV_Dbpos:
	case _GSDATA_TYPE::GVE_BOOLEAN:
		strPrec = "0x15";
		break;
	case _GSDATA_TYPE::GV_Sbpos:
		strPrec = "0x16";
		break;
	case _GSDATA_TYPE::GVE_INT32:
	case _GSDATA_TYPE::GVE_INT32U:
		strPrec = "0x21";
		break;
	case _GSDATA_TYPE::GVE_FLOAT32:
		strPrec = "0x30";
		break;
	case _GSDATA_TYPE::GVE_Octet64:
		strPrec = "0x34";
		break;
	case _GSDATA_TYPE::GVE_Unicode255:
		strPrec = "0x34";
		break;
	case _GSDATA_TYPE::GVE_VisString255:
		strPrec = "0x34";
		break;
	case _GSDATA_TYPE::GVE_Timestamp:
		strPrec = "0x32";
		break;
	case _GSDATA_TYPE::GV_Quality:
		strPrec = "0x32";
		break;

	}

	return strPrec;

}




t_int  ClientDSE_ScdManager::makeSingeEqTable(char *cidfileName,char *tableName,t_char *err)
{

	t_uint8 CpuId = 1;
	t_uint8 InfFun = 1;
	t_uint8 InfInf = 1;
	t_uint8 InfGroupId = 0;
	t_uint8 InfItemId = 0;
	t_uint8 InfEnabled = 1;
	t_uint8 InfDataType = 0;
	t_uint8 InfAttribId = 0;
	QString InfName = "";
	QString InfCode = "";
	QString InfUnit = "";
	t_uint8 InfValueType = 0;
	t_float InfMaxValue = 0.0;
	t_float InfMinValue = 0.0;
	t_float InfStep = 0.0;
	QString InfDefaultValue = "";
	t_float InfFactor = 1;
	QString InfPrecision = "0x30";
	t_int   InfKod = 0;
	t_int   InfType = 0;
	t_int   InfSize = 0;
	t_int   InfNumber = 0;
	t_int   InfDisplevelAutoSend = 255;
	QString InfReference = "";
	t_int   AssociateInfFun  = 0;
	QString Iec61850EntryID = "0,0,0,0,0,0,0,0,";
	QString Iec61850RptID = "0#0#0#0#0#0#0#0#0#0#0#0#0#0#";

	QString driver, host, database, user, pwd;


	m_tableName = QString(tableName);

	if (readConnParam(driver,host,database,user,pwd) == false)
	{
		if (err)
		{
			sprintf(err,"获得数据库的连接参数失败");

		}
		return 5;
	}
	//打开数据库
	//if (connDb("QMYSQL","localhost","fh5000db","root","") == false)
	if (connDb(driver,host,database,user,pwd) == false)
	{
		if (err)
		{
			sprintf(err,"打开数据库失败");

		}
		return 1;
	}

	//删除表中的内容
	deleteTable();

	ClientDSE_ScdManagerImpl impl;

	//读scd cid
	if (impl.readScd(cidfileName,m_subNetList,m_subSta,DECODE_PT,NULL) != 0)
	{
		if (err)
		{
			sprintf(err,"解析%s文件失败",cidfileName);
		}
		return 2;
	}


	bool bRet = true;

	for (int netIndex = 0 ; netIndex < m_subNetList.size() ; netIndex++)
	{
		PSUBNET pSubNet = m_subNetList[netIndex];

		for (int  iedIndex = 0 ; iedIndex < pSubNet->iedList.size() ; iedIndex++)
		{
			PIED pIed = pSubNet->iedList[iedIndex];

			for (int svIndex = 0 ; svIndex < pIed->svList.size() ; svIndex++)
			{
				SVINFO *pSvInfo = pIed->svList[svIndex];

				for (int svChnIndex = 0 ; svChnIndex < pSvInfo->chnList.size() ; svChnIndex++)
				{
					SVCHNINFO *pSvChnInfo = pSvInfo->chnList[svChnIndex];


					InfName       = QString(pSvChnInfo->chnName);
					InfMaxValue   = pSvChnInfo->maxVal;
					InfMinValue   = pSvChnInfo->minVal;
					InfStep       = pSvChnInfo->stepVal;
					InfUnit       = QString(pSvChnInfo->unitStr);
					InfType       = pSvChnInfo->dataType;
					InfSize       = pSvChnInfo->dataSize;

					InfPrecision = getInfPrecision(InfType);
					InfReference = QString(pSvChnInfo->datSetName);  //叶子节点引用
					InfAttribId  = pSvInfo->attribId;
					InfValueType = getInfValueType(InfAttribId);

					if (insertItem( CpuId, InfFun, InfInf,InfGroupId,InfItemId, InfEnabled,InfDataType,
						InfAttribId,InfName,InfCode,InfUnit,InfValueType,InfMaxValue,InfMinValue,
						InfStep,InfDefaultValue,InfFactor,InfPrecision,InfKod,InfType,InfSize,
						InfNumber,InfDisplevelAutoSend,InfReference,AssociateInfFun,Iec61850EntryID,Iec61850RptID) == false)
					{
						bRet = false;
					}

					if (InfInf>= 255)
					{
						InfFun++;
						InfInf = 1;
					}

					InfInf++;
				}

				InfFun++;
				InfInf = 1;

			}

			for (int gsIndex = 0 ; gsIndex < pIed->gsList.size() ; gsIndex++)
			{
				GOOSEINFO *pGsInfo = pIed->gsList[gsIndex];

				for (int gsChnIndex = 0 ; gsChnIndex < pGsInfo->chnList.size() ; gsChnIndex++)
				{
					GSCHNINFO *pGsChnInfo = pGsInfo->chnList[gsChnIndex];

					/*	if (pGsInfo->attribId == ATTRIB_RELAYREC)
					continue;*/

					InfName       = QString(pGsChnInfo->chnName);
					InfType       = pGsChnInfo->dataType;
					InfSize       = pGsChnInfo->dataSize;

					InfPrecision = getInfPrecision(InfType);
					InfReference = QString(pGsChnInfo->datSetName);  //叶子节点引用
					InfAttribId  = pGsInfo->attribId;
					InfValueType = getInfValueType(InfAttribId);

					if (insertItem( CpuId, InfFun, InfInf,InfGroupId,InfItemId, InfEnabled,InfDataType,
						InfAttribId,InfName,InfCode,InfUnit,InfValueType,InfMaxValue,InfMinValue,
						InfStep,InfDefaultValue,InfFactor,InfPrecision,InfKod,InfType,InfSize,
						InfNumber,InfDisplevelAutoSend,InfReference,AssociateInfFun,Iec61850EntryID,Iec61850RptID)  == false)
					{
						bRet = false;
					}

					if (InfInf>= 255)
					{
						InfFun++;
						InfInf = 1;
					}

					InfInf++;
				}



				InfFun++;
				InfInf = 1;
				//插入故障报告
				if ( (QString(pGsInfo->reportCtrl.name) != "") && (pGsInfo->reportCtrl.max >0)  && (QString(pGsInfo->reportCtrl.datSet) != DS_RELAYREC) )
				{


					InfReference = QString("%1/LLN0$BR$%2%3")
						.arg(pGsInfo->reportCtrl.ld)
						.arg(pGsInfo->reportCtrl.name)
						.arg(QString().sprintf("%02d",InfInf));
					Iec61850RptID = QString(pGsInfo->reportCtrl.rptID);
					InfAttribId = ATTRIB_REPORTCTRL;
					InfValueType = getInfValueType(InfAttribId);
					InfName = "故障报告";

					InfMaxValue = 0.0;
					InfMinValue = 0.0;
					InfStep = 0.0;
					InfDefaultValue = "";
					InfFactor = 1;
					InfPrecision = "0x15";
					InfType  = 0;
					InfSize  = 0;
					InfUnit = "";

					if (insertItem( CpuId, InfFun, InfInf,InfGroupId,InfItemId, InfEnabled,InfDataType,
						InfAttribId,InfName,InfCode,InfUnit,InfValueType,InfMaxValue,InfMinValue,
						InfStep,InfDefaultValue,InfFactor,InfPrecision,InfKod,InfType,InfSize,
						InfNumber,InfDisplevelAutoSend,InfReference,AssociateInfFun,Iec61850EntryID,Iec61850RptID)  == false)
					{
						bRet = false;
					}

					Iec61850RptID = "";

					InfFun++;
					InfInf = 1;

				}		
			}
		}
	}


	//关闭数据库
	closeDb();

	if (bRet == false)
	{
		if (err)
		{
			sprintf(err,"插入某条信息失败");
		}
		return 3;
	}

	return 0;
}



t_int  ClientDSE_ScdManager::makeSingeEqTable1(char *cidfileName,char *tableName,t_char *err)
{

	t_uint8 CpuId = 1;
	t_uint8 InfFun = 1;
	t_uint8 InfInf = 1;
	/*t_uint8 InfGroupId = 0;
	t_uint8 InfItemId = 0;*/
	t_uint8 InfEnabled = 1;
	t_uint8 InfDataType = 0;
	t_uint8 InfAttribId = 0;
	QString InfName = "";
	QString InfCode = "";
	QString InfUnit = "";
	t_uint8 InfValueType = 0;
	t_float InfMaxValue = 0.0;
	t_float InfMinValue = 0.0;
	t_float InfStep = 0.0;
	QString InfDefaultValue = "";
	t_float InfFactor = 1;
	QString InfPrecision = "0x30";
	t_int   InfKod = 0;
	t_int   InfType = 0;
	t_int   InfSize = 0;
	t_int   InfNumber = 0;
	t_int   InfDisplevelAutoSend = 255;
	QString InfReference = "";
	t_int   AssociateInfFun  = 0;
	QString Iec61850EntryID = "0,0,0,0,0,0,0,0,";
	//QString Iec61850RptID = "0#0#0#0#0#0#0#0#0#0#0#0#0#0#";

	QString driver, host, database, user, pwd;


	m_tableName = QString(tableName);

	if (readConnParam1(driver,host,database,user,pwd) == false)
	{
		if (err)
		{
			sprintf(err,"获得数据库的连接参数失败");

		}
		return 5;
	}
	//打开数据库
	//if (connDb("QMYSQL","localhost","fh5000db","root","") == false)
	if (connDb(driver,host,database,user,pwd) == false)
	{
		if (err)
		{
			sprintf(err,"打开数据库失败");

		}
		return 1;
	}

	//删除表中的内容
	deleteTable();

	ClientDSE_ScdManagerImpl impl;

	//读scd cid
	if (impl.readScd(cidfileName,m_subNetList,m_subSta,DECODE_PT,NULL) != 0)
	{
		if (err)
		{
			sprintf(err,"解析%s文件失败",cidfileName);
		}
		return 2;
	}


	bool bRet = true;

	for (int netIndex = 0 ; netIndex < m_subNetList.size() ; netIndex++)
	{
		PSUBNET pSubNet = m_subNetList[netIndex];

		for (int  iedIndex = 0 ; iedIndex < pSubNet->iedList.size() ; iedIndex++)
		{
			PIED pIed = pSubNet->iedList[iedIndex];

			for (int svIndex = 0 ; svIndex < pIed->svList.size() ; svIndex++)
			{
				SVINFO *pSvInfo = pIed->svList[svIndex];

				for (int svChnIndex = 0 ; svChnIndex < pSvInfo->chnList.size() ; svChnIndex++)
				{
					SVCHNINFO *pSvChnInfo = pSvInfo->chnList[svChnIndex];


					InfName       = QString(pSvChnInfo->chnName);
					InfMaxValue   = pSvChnInfo->maxVal;
					InfMinValue   = pSvChnInfo->minVal;
					InfStep       = pSvChnInfo->stepVal;
					InfUnit       = QString(pSvChnInfo->unitStr);
					InfType       = pSvChnInfo->dataType;
					InfSize       = pSvChnInfo->dataSize;

					InfPrecision = getInfPrecision(InfType);
					InfReference = QString(pSvChnInfo->datSetName);  //叶子节点引用
					InfAttribId  = pSvInfo->attribId;
					InfValueType = getInfValueType(InfAttribId);

					if (insertItem1( CpuId, InfFun, InfInf, InfEnabled,InfDataType,
						InfAttribId,InfName,InfCode,InfUnit,InfValueType,InfMaxValue,InfMinValue,
						InfStep,InfDefaultValue,InfFactor,InfPrecision,InfKod,InfType,InfSize,
						InfNumber,InfDisplevelAutoSend,InfReference,AssociateInfFun,Iec61850EntryID) == false)
					{
						bRet = false;
					}

					if (InfInf>= 255)
					{
						InfFun++;
						InfInf = 1;
					}

					InfInf++;
				}

				InfFun++;
				InfInf = 1;

			}

			for (int gsIndex = 0 ; gsIndex < pIed->gsList.size() ; gsIndex++)
			{
				GOOSEINFO *pGsInfo = pIed->gsList[gsIndex];

				for (int gsChnIndex = 0 ; gsChnIndex < pGsInfo->chnList.size() ; gsChnIndex++)
				{
					GSCHNINFO *pGsChnInfo = pGsInfo->chnList[gsChnIndex];

					/*	if (pGsInfo->attribId == ATTRIB_RELAYREC)
					continue;*/

					InfName       = QString(pGsChnInfo->chnName);
					InfType       = pGsChnInfo->dataType;
					InfSize       = pGsChnInfo->dataSize;

					InfPrecision = getInfPrecision(InfType);
					InfReference = QString(pGsChnInfo->datSetName);  //叶子节点引用
					InfAttribId  = pGsInfo->attribId;
					InfValueType = getInfValueType(InfAttribId);

					if (insertItem1( CpuId, InfFun, InfInf, InfEnabled,InfDataType,
						InfAttribId,InfName,InfCode,InfUnit,InfValueType,InfMaxValue,InfMinValue,
						InfStep,InfDefaultValue,InfFactor,InfPrecision,InfKod,InfType,InfSize,
						InfNumber,InfDisplevelAutoSend,InfReference,AssociateInfFun,Iec61850EntryID)  == false)
					{
						bRet = false;
					}

					if (InfInf>= 255)
					{
						InfFun++;
						InfInf = 1;
					}

					InfInf++;
				}



				InfFun++;
				InfInf = 1;

			}
		}
	}


	//关闭数据库
	closeDb();

	if (bRet == false)
	{
		if (err)
		{
			sprintf(err,"插入某条信息失败");
		}
		return 3;
	}

	return 0;
}


