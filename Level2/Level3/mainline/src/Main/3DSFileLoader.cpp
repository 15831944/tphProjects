#include "StdAfx.h"
#include ".\3dsfileloader.h"
#include "./3dmodel.h"
#include <cmath>

int buffer[500000];
//  tChunk
C3DSFileLoader::C3DSFileLoader()
{
	m_CurrentChunk = new tChunk;				// 
	m_TempChunk = new tChunk;					// 
}

//  3ds
bool C3DSFileLoader::LoadModel(CString filename,C3dModel & tmodel)
{
	char strMessage[255] = {0};

	// 3ds
	m_FilePointer = fopen(filename, "rb");

	// 
	if(!m_FilePointer ) 
	{
		sprintf(strMessage, "Unable to find the file: %s!", filename);
		//MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// 
	ReadChunk(m_CurrentChunk);

	// be sure is a 3ds file
	if (m_CurrentChunk->ID != PRIMARY)
	{
		sprintf(strMessage, "Unable to load PRIMARY chuck from file: %s!", filename);
		//MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// read data,ProcessNextChunk()

	ProcessNextChunk(&tmodel.g_3DModel, m_CurrentChunk);

	// 
	ComputeNormals(&tmodel.g_3DModel);

	// 
	CleanUp();

	return true;
}


// ,
void C3DSFileLoader::CleanUp()
{

	fclose(m_FilePointer);						// 
	delete m_CurrentChunk;						// 
	delete m_TempChunk;							// 
}

//  3ds
void C3DSFileLoader::ProcessNextChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	t3DObject newObject = {0};					// 
	tMaterialInfo newTexture = {0};				// 
	unsigned int version = 0;					// 
	memset(buffer,0, sizeof(buffer));

	m_CurrentChunk = new tChunk;				// 		

	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{

		ReadChunk(m_CurrentChunk);

		switch (m_CurrentChunk->ID)
		{
		case C3DSVERSION:							

			m_CurrentChunk->bytesRead += fread(&version, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);

			if (version > 0x03)
				NULL;//MessageBox(NULL, "This 3DS file is over version 3 so it may load incorrectly", "Warning", MB_OK);
			break;

		case OBJECTINFO:						

			ReadChunk(m_TempChunk);

			m_TempChunk->bytesRead += fread(&version, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);

			m_CurrentChunk->bytesRead += m_TempChunk->bytesRead;

			ProcessNextChunk(pModel, m_CurrentChunk);
			break;

		case MATERIAL:							

			pModel->numOfMaterials++;

			pModel->pMaterials.push_back(newTexture);

			ProcessNextMaterialChunk(pModel, m_CurrentChunk);
			break;

		case OBJECT:							

			pModel->numOfObjects++;

			pModel->pObject.push_back(newObject);

			memset(&(pModel->pObject[pModel->numOfObjects - 1]), 0, sizeof(t3DObject));

			m_CurrentChunk->bytesRead += GetString(pModel->pObject[pModel->numOfObjects - 1].strName);

			ProcessNextObjectChunk(pModel, &(pModel->pObject[pModel->numOfObjects - 1]), m_CurrentChunk);
			break;

		case EDITKEYFRAME:

			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		default: 

			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}


void C3DSFileLoader::ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	memset(buffer,0, sizeof(buffer));					

	m_CurrentChunk = new tChunk;

	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		ReadChunk(m_CurrentChunk);

		switch (m_CurrentChunk->ID)
		{
		case OBJECT_MESH:					

			ProcessNextObjectChunk(pModel, pObject, m_CurrentChunk);
			break;

		case OBJECT_VERTICES:				
			ReadVertices(pObject, m_CurrentChunk);
			break;

		case OBJECT_FACES:					
			ReadVertexIndices(pObject, m_CurrentChunk);
			break;

		case OBJECT_MATERIAL:				

			ReadObjectMaterial(pModel, pObject, m_CurrentChunk);			
			break;

		case OBJECT_UV:				
			ReadUVCoordinates(pObject, m_CurrentChunk);
			break;

		default:  

			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}

void C3DSFileLoader::ProcessNextMaterialChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	memset(buffer,0, sizeof(buffer));			
	m_CurrentChunk = new tChunk;

	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		ReadChunk(m_CurrentChunk);

		switch (m_CurrentChunk->ID)
		{
		case MATNAME:							// 

			m_CurrentChunk->bytesRead +=  fread(pModel->pMaterials[pModel->numOfMaterials - 1].strName, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		case MATDIFFUSE:						// 
			ReadColorChunk(&(pModel->pMaterials[pModel->numOfMaterials - 1]), m_CurrentChunk);
			break;

		case MATMAP:							// 

			ProcessNextMaterialChunk(pModel, m_CurrentChunk);
			break;

		case MATMAPFILE:						// 

			m_CurrentChunk->bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strFile, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		default:  

			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}

void C3DSFileLoader::ReadChunk(tChunk *pChunk)
{
	pChunk->bytesRead = fread(&pChunk->ID, 1, 2, m_FilePointer);

	pChunk->bytesRead += fread(&pChunk->length, 1, 4, m_FilePointer);
}

int C3DSFileLoader::GetString(char *pBuffer)
{
	int index = 0;

	fread(pBuffer, 1, 1, m_FilePointer);

	while (*(pBuffer + index++) != 0) {

		fread(pBuffer + index, 1, 1, m_FilePointer);
	}

	return strlen(pBuffer) + 1;
}


void C3DSFileLoader::ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk)
{
	ReadChunk(m_TempChunk);

	m_TempChunk->bytesRead += fread(pMaterial->color, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);

	pChunk->bytesRead += m_TempChunk->bytesRead;
}

void C3DSFileLoader::ReadVertexIndices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	unsigned short index = 0;					

	pPreviousChunk->bytesRead += fread(&pObject->numOfFaces, 1, 2, m_FilePointer);

	pObject->pFaces = new tFace [pObject->numOfFaces];
	memset(pObject->pFaces, 0, sizeof(tFace) * pObject->numOfFaces);

	for(int i = 0; i < pObject->numOfFaces; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			pPreviousChunk->bytesRead += fread(&index, 1, sizeof(index), m_FilePointer);

			if(j < 3)
			{
				pObject->pFaces[i].vertIndex[j] = index;
			}
		}
	}
}


void C3DSFileLoader::ReadUVCoordinates(t3DObject *pObject, tChunk *pPreviousChunk)
{

	pPreviousChunk->bytesRead += fread(&pObject->numTexVertex, 1, 2, m_FilePointer);

	// UV
	pObject->pTexVerts = new CVector2 [pObject->numTexVertex];

	// 
	pPreviousChunk->bytesRead += fread(pObject->pTexVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}

//  
void C3DSFileLoader::ReadVertices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	// £¬¡£

	// 
	pPreviousChunk->bytesRead += fread(&(pObject->numOfVerts), 1, 2, m_FilePointer);

	// £¬
	pObject->pVerts = new CVector3 [pObject->numOfVerts];
	memset(pObject->pVerts, 0, sizeof(CVector3) * pObject->numOfVerts);

	// 
	pPreviousChunk->bytesRead += fread(pObject->pVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);


	// travser all vertex
	for(int i = 0; i < pObject->numOfVerts; i++)
	{
		// Y
		float fTempY = pObject->pVerts[i].y;

		// Z
		pObject->pVerts[i].y = pObject->pVerts[i].z;

		// Z-Y 
		pObject->pVerts[i].z = -fTempY;
	}
}


//  
void C3DSFileLoader::ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	char strMaterial[255] = {0};			// 
	memset(buffer,0, sizeof(buffer));

	// 
	// 
	pPreviousChunk->bytesRead += GetString(strMaterial);

	// 
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{
		//
		if(strcmp(strMaterial, pModel->pMaterials[i].strName) == 0)
		{
			// ID
			pObject->materialID = i;

			//
			if(strlen(pModel->pMaterials[i].strFile) > 0) {
				// 
				pObject->bHasTexture = true;
			}	
			break;
		}
		else
		{
			//no material ID =-1
			pObject->materialID = -1;
		}
	}

	pPreviousChunk->bytesRead += fread(buffer, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}			



//  
// 
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

// 
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;							

	vVector.x = vPoint1.x - vPoint2.x;			
	vVector.y = vPoint1.y - vPoint2.y;			
	vVector.z = vPoint1.z - vPoint2.z;			

	return vVector;								
}

// 
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vResult;							

	vResult.x = vVector2.x + vVector1.x;		
	vResult.y = vVector2.y + vVector1.y;		
	vResult.z = vVector2.z + vVector1.z;		

	return vResult;								
}

// 
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;							

	vResult.x = vVector1.x / Scaler;			
	vResult.y = vVector1.y / Scaler;			
	vResult.z = vVector1.z / Scaler;			

	return vResult;								
}

// 
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;								

	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));

	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));

	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;								
}

// 
CVector3 Normalize(CVector3 vNormal)
{
	double Magnitude;							

	Magnitude = Mag(vNormal);					// 

	vNormal.x /= (float)Magnitude;				
	vNormal.y /= (float)Magnitude;				
	vNormal.z /= (float)Magnitude;				

	return vNormal;								
}

//  
void C3DSFileLoader::ComputeNormals(t3DModel *pModel)
{
	CVector3 vVector1, vVector2, vNormal, vPoly[3];

	// £¬
	if(pModel->numOfObjects <= 0)
		return;

	// 
	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		// 
		t3DObject *pObject = &(pModel->pObject[index]);

		// 
		CVector3 *pNormals		= new CVector3 [pObject->numOfFaces];
		CVector3 *pTempNormals	= new CVector3 [pObject->numOfFaces];
		pObject->pNormals		= new CVector3 [pObject->numOfVerts];

		// 
		for(int i=0; i < pObject->numOfFaces; i++)
		{												
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			// 

			vVector1 = Vector(vPoly[0], vPoly[2]);		// 
			vVector2 = Vector(vPoly[2], vPoly[1]);		// 

			vNormal  = Cross(vVector1, vVector2);		// 
			pTempNormals[i] = vNormal;					// 
			vNormal  = Normalize(vNormal);				// 

			pNormals[i] = vNormal;						// 
		}

		//  
		CVector3 vSum = {0.0, 0.0, 0.0};
		CVector3 vZero = vSum;
		int shared=0;
		// 
		for (int i = 0; i < pObject->numOfVerts; i++)			
		{
			for (int j = 0; j < pObject->numOfFaces; j++)	// 
			{												// 
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i)
				{
					vSum = AddVector(vSum, pTempNormals[j]);
					shared++;								
				}
			}      

			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));

			// 
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]);	

			vSum = vZero;								
			shared = 0;										
		}

		//
		delete [] pTempNormals;
		delete [] pNormals;
	}
}
