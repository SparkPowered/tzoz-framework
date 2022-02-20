#pragma once

namespace css
{
	class RecvTable;
	class RecvProp;

	typedef enum
	{
		DPT_Int=0,
		DPT_Float,
		DPT_Vector,
		DPT_VectorXY,
		DPT_String,
		DPT_Array,
		DPT_DataTable,
		//DPT_Int64,
		DPT_NUMSendPropTypes,
	} SendPropType;

	class DVariant {
	public:
		union
		{
			float m_Float;
			long m_Int;
			char *m_pString;
			void *m_pData; // For DataTables.
			float m_Vector[3];
			//long long m_Int64;
		};
	
		SendPropType m_Type;
	};

	class CRecvProxyData {
	public:
		const RecvProp *m_pRecvProp;
		DVariant m_Value;
		int m_iElement;
		int m_ObjectID;
	};

	typedef void (*RecvVarProxyFn)(const CRecvProxyData *pData, void *pStruct, void *pOut);
	typedef void (*ArrayLengthRecvProxyFn)(void *pStruct, int objectID, int currentArrayLength);
	typedef void (*DataTableRecvVarProxyFn)(const RecvProp *pProp, void **pOut, void *pData, int objectID);

	class RecvProp {
	public:
		const char *m_pVarName;
		SendPropType m_RecvType;
		int	m_Flags;
		int	m_StringBufferSize;
		bool m_bInsideArray;
		const void *m_pExtraData;
		RecvProp *m_pArrayProp;
		ArrayLengthRecvProxyFn m_ArrayLengthProxyFn;
		RecvVarProxyFn m_ProxyFn;
		DataTableRecvVarProxyFn m_DataTableProxyFn;
		RecvTable *m_pDataTable;
		int m_Offset;
		int	m_ElementStride;
		int	m_nElements;
		const char *m_pParentArrayPropName;
	};

	class RecvTable {
	public:
		RecvProp *m_pProps;
		int m_nProps;
		void *m_pDecoder;
		const char *m_pNetTableName;
		bool m_bInitialized;
		bool m_bInMainList;
	};

	class ClientClass {
	public:
		void *m_pCreateFn;
		void *m_pCreateEventFn;
		const char *m_pNetworkName;
		RecvTable *m_pRecvTable;
		ClientClass *m_pNext;
		int m_ClassID;
	};
};