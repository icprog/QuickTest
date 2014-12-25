//#ifndef CSOFTPLATE_H
//#define CSOFTPLATE_H

//#include <QObject>

//class CSoftPlate
//{
//public:
//    CSoftPlate();
//    ~CSoftPlate();

//    /* Interface */
//    bool isEnable() {return bEnable;}
//    QString describe() {return strDescribe;}
//    QString reference() {return strReference;}

//protected:

//private:
//    QString     strDescribe;  //纵联差动保护(软)
//    bool        bEnable;      //状态使能
//    QString     strReference; //CSC103BDNPROT/LLN0$ST$SoftEna2$stVal
//};

//class CSoftPlateGroup : QObject
//{
//    Q_OBJECT
//public:
//    CSoftPlateGroup();
//    ~CSoftPlateGroup();

//    void append(CSoftPlate * const &t);
//    void append(const QList<CSoftPlate *> &t);
//    bool remove(CSoftPlate * const &t);
//    bool remove(const QString &reference);
//    void clear();

//    CSoftPlate * at(int i) const;

//protected:
//    QList<CSoftPlate*> m_softPlateList;
//};
//#endif // CSOFTPLATE_H
