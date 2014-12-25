#include "clientdatadefine.h"
#include "clientscdphase.h"


#define TYPE_UNKNOWN			-1
#define TYPE_ARR_START	        1
#define TYPE_STR_START	        2
#define	TYPE_BOOL		        3
#define	TYPE_BIT_STRING	        4
#define	TYPE_INTEGER			5
#define	TYPE_UNSIGNED	        6
#define	TYPE_FLOATING_POINT		7
#define	TYPE_OCTET_STRING	    9
#define	TYPE_VISIBLE_STRING		10
#define	TYPE_GENERAL_TIME	    11
#define	TYPE_BINARY_TIME	    12
#define	TYPE_BCD		        13
#define	TYPE_BOOLEANARRAY	    14
#define	TYPE_UTC_TIME	        17
#define	TYPE_UTF8_STRING		16


void getTypeStr(int data_type ,t_char *typeStr)
{


	switch (data_type)
	{
	case TYPE_BOOL:
        sprintf(typeStr,"布尔类型");
        break;
	case TYPE_STR_START:
		 sprintf(typeStr,"结构");
		break;
    case TYPE_BIT_STRING:
        {
          sprintf(typeStr,"比特串");
        }
		break;
    case TYPE_INTEGER:
        sprintf(typeStr,"整数型");
		break;
	case TYPE_UNSIGNED:
        sprintf(typeStr,"无符号整数型");
		break;
	case TYPE_FLOATING_POINT:
        sprintf(typeStr,"浮点型及双精度型");
		break;
    case TYPE_OCTET_STRING:
		sprintf(typeStr,"字节串型");
		break;
    case TYPE_VISIBLE_STRING:
        sprintf(typeStr,"可见字符串型");
        break;
	case TYPE_GENERAL_TIME:
        {
           sprintf(typeStr,"普通时间类型");
        }
		break;
	case TYPE_BINARY_TIME:
		 sprintf(typeStr,"二元时间类型");
		break;
	case TYPE_BCD:
		 sprintf(typeStr,"BCD码类型");
		break;
    case TYPE_BOOLEANARRAY:
		 sprintf(typeStr,"布尔数组类型");
        break;
    case TYPE_UTF8_STRING:
         sprintf(typeStr,"Unicode字符串类型"); 		
		break;
	case TYPE_UTC_TIME://utctime
        {
             sprintf(typeStr,"UTC时间类型"); 		
        }
		break;

	default:
		{
			  sprintf(typeStr,""); 	
		}
		break;
	}
}


void getUnit(int unitOrd,char *unitStr)
{
	switch(unitOrd)
	{

	case 1:
		sprintf(unitStr,"");
		break;
	case 2:
         sprintf(unitStr,"m");
		 break;
 	case 3:
     sprintf(unitStr,"kg");
	 break;
 	case 4:
     sprintf(unitStr,"s");
	 break;
 	case 5:
     sprintf(unitStr,"A");
	 break;
 	case 6:
     sprintf(unitStr,"K");
	 break;
 	case 7:
     sprintf(unitStr,"mol");
	 break;
 	case 8:
     sprintf(unitStr,"cd");
	 break;
 	case 9:
     sprintf(unitStr,"deg");
	 break;
 	case 10:
     sprintf(unitStr,"rad");
	 break;
 	case 11:
     sprintf(unitStr,"sr");
	 break;
 	case 21:
     sprintf(unitStr,"Gy");
	 break;
 	case 22:
     sprintf(unitStr,"q");
	 break;
 	case 23:
     sprintf(unitStr,"°C");
	 break;
 	case 24:
     sprintf(unitStr,"Sv");
	 break;
 	case 25:
     sprintf(unitStr,"F");
	 break;
 	case 26:
     sprintf(unitStr,"C");
	 break;
 	case 27:
     sprintf(unitStr,"S");
	 break;
 	case 28:
     sprintf(unitStr,"H");
	 break;
 	case 29:
     sprintf(unitStr,"V");
	 break;
 	case 30:
     sprintf(unitStr,"ohm");
	 break;
 	case 31:
     sprintf(unitStr,"J");
	 break;
 	case 32:
     sprintf(unitStr,"N");
	 break;
 	case 33:
     sprintf(unitStr,"Hz");
	 break;
 	case 34:
     sprintf(unitStr,"lx");
	 break;
 	case 35:
     sprintf(unitStr,"Lm");
	 break;
 	case 36:
     sprintf(unitStr,"Wb");
	 break;
 	case 37:
     sprintf(unitStr,"T");
	 break;
 	case 38:
     sprintf(unitStr,"W");
	 break;
 	case 39:
     sprintf(unitStr,"Pa");
	 break;
 	case 40:
     sprintf(unitStr,"");
	 break;
 	case 41:
     sprintf(unitStr,"m2");
	 break;
 	case 42:
     sprintf(unitStr,"m3");
	 break;
 	case 43:
     sprintf(unitStr,"m/s");
	 break;
	 case 44:
     sprintf(unitStr,"m/s2");
	 break;
	 case 45:
     sprintf(unitStr,"m3/s");
	 break;
	 case 46:
     sprintf(unitStr,"m/m3");
	 break;
	  case 47:
     sprintf(unitStr,"M");
	 break;
	  case 48:
     sprintf(unitStr,"kg/m3");
	 break;
	  case 49:
     sprintf(unitStr,"m2/s");
	 break;
	 case 50:
     sprintf(unitStr,"W/m K");
	 break;
     case 51:
     sprintf(unitStr,"J/K");
	 break;
	  case 52:
     sprintf(unitStr,"ppm");
	 break;
	  case 53:
     sprintf(unitStr,"1/s");
	 break;
	 case 54:
     sprintf(unitStr,"rad/s");
	 break;
	  case 61:
     sprintf(unitStr,"VA");
	 break;
	  case 62:
     sprintf(unitStr,"Watts");
	 break;
	 case 63:
     sprintf(unitStr,"VAr");
	 break;
	 case 64:
     sprintf(unitStr,"phi");
	 break;
	  case 65:
     sprintf(unitStr,"cos(phi)");
	 break;
	 case 66:
     sprintf(unitStr,"Vs");
	 break;
	 case 67:
     sprintf(unitStr,"V2");
	 break;
	 case 68:
     sprintf(unitStr,"As");
	 break;
	  case 69:
     sprintf(unitStr,"A2");
	 break;
	   case 70:
     sprintf(unitStr,"A2t");
	 break;
	 case 71:
     sprintf(unitStr,"Vah");
	 break;
    case 72:
     sprintf(unitStr,"Wh");
	 break;
	 case 73:
     sprintf(unitStr,"VArh");
	 break;
     case 74:
     sprintf(unitStr,"V/Hz");
	 break;
	 default:
		 sprintf(unitStr,"");
	 break;

	}

}

void getMultiplier(int multiOrd,char *multiStr)
{

	switch(multiOrd)
	{

	case -24:
		sprintf(multiStr,"y");
		break;
	case -21:
         sprintf(multiStr,"z");
		 break;
 	case -18:
		sprintf(multiStr,"a");
		break;
	case -15:
		sprintf(multiStr,"f");
		break;
	case -12:
		sprintf(multiStr,"p");
		break;
	 case -9:
		sprintf(multiStr,"n");
		break;
	 case -6:
		sprintf(multiStr,"u");
		break;
	 case -3:
		sprintf(multiStr,"m");
		break;
	 case -2:
		sprintf(multiStr,"c");
		break;
	 case -1:
		sprintf(multiStr,"d");
		break;
	 case 1:
		sprintf(multiStr,"da");
		break;
	 case 2:
		sprintf(multiStr,"h");
		break;
	 case 3:
		sprintf(multiStr,"k");
		break;
	 case 6:
		sprintf(multiStr,"M");
		break;
	case 9:
		sprintf(multiStr,"G");
		break;
	case 12:
		sprintf(multiStr,"T");
		break;
	case 15:
		sprintf(multiStr,"P");
		break;
	case 18:
		sprintf(multiStr,"E");
		break;
	case 21:
		sprintf(multiStr,"Z");
		break;
	case 24:
		sprintf(multiStr,"Y");
		break;
	default:
		sprintf(multiStr,"");
		break;
	}


}
