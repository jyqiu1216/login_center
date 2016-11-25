/************************************************************
 * Copyright (C), 1998-2009, Tencent Technology Company Limited
 *
 * �ļ�����: FieldProperty.h
 * ����: allenhuang
 * ����: 2009/04/23
 * �汾: 1.0
 * ģ������: �ֶ�������Ϣ
 * �����:
 *      CFieldProperty
 * �޸���ʷ:
 *      <author>        <time>       <version >   <desc>
 *      allenhuang   2009/04/23          1.0        ����
 *************************************************************/

#ifndef __KINGDOM_FIELD_PROPERTY_H__
#define __KINGDOM_FIELD_PROPERTY_H__

#include "base/common/wtse_std_header.h"
#include "base/conf/tinyxml.h"
#include "aws_table_common.h"

#pragma pack(1)
// ����ֶ���Ŀ
#define        MAX_FIELD_NUM    (128)
// ��������������
#define        MAX_ALIAS_NUM    (1 << 16)
// �������Ŀ�����ʽ��Ŀ
#define			MAX_OPEN_TYPE_NUM	(10)
// ������������������Ŀ
#define			MAX_UNION_INDEX_FLD_NUM	(10)
// �������ı��ض���������Ŀ
#define			MAX_LOCAL_SEC_INDEX_NUM	(5)
#define			MAX_GLOBAL_SEC_INDEX_NUM	(5)
// ���屾�ض�����������ӳ���ֶ���Ŀ
#define			MAX_LOCAL_PROJECTION_FIELD_NUM	(20)

#define         MAX_OPEN_TYPE_ENUM_STR_LEN  (64)

//////////////////////////////////////////////////////////////////////////
// ��ֵ����
enum ENumType
{
    NUM_TYPE_UINT8 = 1,         // TUINT8
    NUM_TYPE_UINT16 = 2,        // TUINT16
    NUM_TYPE_UINT32 = 4,        // TUINT32
    NUM_TYPE_UINT64 = 8,        // TUINT64
    NUM_TYPE_MAX                // ��ֵ�������ֵ
};
// ��ֵ���Ͷ�Ӧ��������Ϣ
static const TCHAR g_aaszNumTypeStr[][16] =
{
    "",                 // 0
    "TUINT8",           // 1
    "TUINT16",          // 2
    "",                 // 3
    "TUINT32",          // 4
    "",                 // 5
    "",                 // 6
    "",                 // 7
    "TUINT64"           // 8
};
//////////////////////////////////////////////////////////////////////////
// �ֶ�����
struct SFieldProperty
{
    // �ֶα��
    TUINT16     m_uwFieldNo;
    // �ֶ�����
    EFieldType  m_enFieldType;
    // ��ֵ����,���ֵֻ������ֵ�ֶ�ʱ��Ч
    ENumType    m_enNumType;
    // �ֶγ���
    TUINT32     m_udwFieldLen;
    // ������ʽ
    TUINT8      m_ucIndexType;
	// �ֶ����ƣ�Ӧ����mysql�����ƶ���һ��
	TCHAR		m_szTitle[64];

    // �������ֶνṹ������
    TCHAR       m_szBinStructName[64];
    // �������ֶνṹ������С
    TUINT32     m_udwBinStructMaxSize;

    SFieldProperty()
    {
        m_uwFieldNo = 0;
        m_enFieldType = FIELD_TYPE_UNKNOWN;
        m_enNumType = NUM_TYPE_MAX;
        m_udwFieldLen = 0;
        m_ucIndexType = INDEX_TYPE_NONE;

		m_szTitle[0] = 0;

        m_szBinStructName[0] = 0;
        m_udwBinStructMaxSize = 0;
    }
};
//////////////////////////////////////////////////////////////////////////
// �������÷�ʽ
struct SOpenIndexType
{
	// ��ʽ���
	TUINT16	m_uwTypeNo;
	// ��������
	TCHAR	m_szIndexName[64];
	
	// ��Ҫ���ص��ֶα��
	TUINT32 m_udwRetFieldNum;
	TUINT16	m_auwRetFieldNo[MAX_FIELD_NUM];
	TUINT8	m_aucFieldRetFlag[MAX_FIELD_NUM];

    // �򿪷�ʽ�ı����������Զ�����enum
    TCHAR   m_szEType[MAX_OPEN_TYPE_ENUM_STR_LEN];

	SOpenIndexType()
	{
		m_uwTypeNo = 0;
		m_szIndexName[0] = 0;
		m_udwRetFieldNum = 0;
        m_szEType[0] = 0;
		memset((char*)m_aucFieldRetFlag, 0, sizeof(TUINT8)*MAX_FIELD_NUM);
	}
};

// ���ض���������Ϣ
struct SLocalIndexType
{
	// ��ʽ���
	TUINT16	m_uwTypeNo;
	// ��������
	TCHAR	m_szIndexName[64];
	// Ŀ�������ֶα��
	TUINT16 m_uwIndexField;

	// �ֶ�ӳ�䷽ʽ
	TUINT8	m_ucProjectionType;
	// �ֶ�ӳ���б�
	TUINT32	m_udwProjectionFieldNum;
	TUINT16	m_auwProjectionFieldList[MAX_LOCAL_PROJECTION_FIELD_NUM];

	SLocalIndexType()
	{
		m_uwTypeNo = 0;
		m_szIndexName[0] = 0;
		m_uwIndexField = 0;
		m_ucProjectionType = LOCAL_PROJECTION_TYPE__KEYS_ONLY;
		m_udwProjectionFieldNum = 0;
	}
};

// ȫ�ֶ���������Ϣ
struct SGlobalIndexType
{
    // ��ʽ���
    TUINT16	m_uwTypeNo;
    // ��������
    TCHAR	m_szIndexName[64];
    // Ŀ�������ֶα��
    TINT16 m_wHashIndexField;
    TINT16 m_wRangeIndexField;

    // �ֶ�ӳ�䷽ʽ
    TUINT8	m_ucProjectionType;
    // �ֶ�ӳ���б�
    TUINT32	m_udwProjectionFieldNum;
    TUINT16	m_auwProjectionFieldList[MAX_LOCAL_PROJECTION_FIELD_NUM];

    // index�Ķ�д��ֵ
    TUINT32				m_udwReadUnits;
    TUINT32				m_udwWriteUnits;

    SGlobalIndexType()
    {
        m_uwTypeNo = 0;
        m_szIndexName[0] = 0;
        m_wHashIndexField = -1;
        m_wRangeIndexField = -1;
        m_ucProjectionType = LOCAL_PROJECTION_TYPE__KEYS_ONLY;
        m_udwProjectionFieldNum = 0;
        m_udwReadUnits = 0;
        m_udwWriteUnits = 0;
    }
};

//////////////////////////////////////////////////////////////////////////
/************************************************************************
  ����������
  ��ģ���ṩ�ֶ�������Ϣ�Ķ�ȡ��
 ************************************************************************/
class CFieldProperty
{
public:
    CFieldProperty();
    ~CFieldProperty();
public:
    /************************************************************************
      ���ܣ�
      ��ʼ������
      ���룺
		szConfFile        :�����ļ�
		�����
		TRUE    ���ɹ�
		FALSE   ��ʧ��
		Example��

     ************************************************************************/
    TBOOL        Init( const char* szConfFile );
    /************************************************************************
      ���ܣ�
      �ͷ���Դ
      ���룺

      �����
      TRUE    ���ɹ�
      FALSE   ��ʧ��
      Example��

     ************************************************************************/
    TBOOL        Release();
    /************************************************************************
     * ���ܣ�
     *  ��ȡָ���ֶ���Ϣ
     * ���룺
     *  uwFieldNo        :�ֶα��
     *  pstField        ���ֶ�����
     * �����
     *  TRUE    ���ɹ�
     *  FALSE   ��ʧ��
     * Example��
     ***********************************************************************/
    TBOOL        GetFieldInfo( TUINT16 uwFieldNo, SFieldProperty* &pstField );

	/************************************************************************
     * ���ܣ�
     *  ��ȡָ����������ʽ����Ϣ
     * ���룺
     *  uwType				:��ʽ���
     *  pstOpenType         :��ʽ����
     * �����
     *  TRUE    ���ɹ�
     *  FALSE   ��ʧ��
     * Example��
     ***********************************************************************/
    TBOOL        GetOpenTypeInfo( TUINT16 uwType, SOpenIndexType* &pstOpenType );
   
    /************************************************************************
     * ���ܣ�
     *  ��ȡ�����ֶ�����
     * ���룺
     *  ppstField        ���ֶ������б�
     *  uwFieldNum        ���ֶ���Ŀ
     * �����
     *  TRUE    ���ɹ�
     *  FALSE   ��ʧ��
     * Example��
     ************************************************************************/
    TBOOL        GetAllField( SFieldProperty** &ppstField,TUINT16& uwFieldNum );
    /************************************************************************
     * ���ܣ�
     *  ��ȡ�ֶα�����Ӧ���ֶα��
     * ���룺
     *  pszAlias        ���ֶα���
     * �����
     *  �ֶα��
     * Example��
     ************************************************************************/
    TUINT16        AliasToFieldNo( const TCHAR* pszAlias );
public:
    // װ���ֶ��б�
    TBOOL        LoadFieldList( TiXmlHandle *pDocConf );
	// װ�ط�ʽ�б�
	TBOOL		 LoadOpenTypeList( TiXmlHandle *pDocConf );
	// װ�ر��ض��������б�
	TBOOL		 LoadLocalIndexList( TiXmlHandle *pDocConf );
    // װ��Global���������б�
    TBOOL		 LoadGlobalIndexList( TiXmlHandle *pDocConf );
public:
    // �����ֶ�
    TUINT16                m_uwAllFieldNum;
    SFieldProperty*        m_pastAllFieldProperty[MAX_FIELD_NUM];

	// ���е�open_index����
	TUINT16					m_uwALLOpenTypeNum;
	SOpenIndexType*			m_pastAllOpenType[MAX_OPEN_TYPE_NUM];
    
    // ������Ӧ���ֶα��
    TUINT16*            m_puwAliasToFieldNo;

	// table name
	TCHAR				m_szTableName[64];
	TUINT32				m_udwReadUnits;
	TUINT32				m_udwWriteUnits;
	TUINT8				m_ucMutleTableFlag;
    TUINT32             m_udwTableType;
    TUINT32             m_udwDependFldNo;
    TUINT32             m_udwSplitSize;

	//andy@20160527: new add for cache module
	TUINT32				m_udwCacheMode;
	TUINT32				m_udwRouteFld;
	TUINT64				m_uddwRouteThrd;

	// hash_key
	SFieldProperty*			m_pstHashKey;
	SFieldProperty*			m_pstRangeKey;

	// local index
	TUINT16					m_uwLocalIndexNum;
	SLocalIndexType*		m_pastLocalIndex[MAX_LOCAL_SEC_INDEX_NUM];

    // global index
    TUINT16					m_uwGlobalIndexNum;
    SGlobalIndexType*		m_pastGlobalIndex[MAX_GLOBAL_SEC_INDEX_NUM];

    // ��¼���������ֶ�ֵ
    map<unsigned int, int> m_HashFldMap;
    map<unsigned int, int> m_RangeFldMap;

public:
	int	GetTableInfo(TableDesc *poTableInfo);
};

#pragma pack()

#endif

