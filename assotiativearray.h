
/******************************************************
Copyright © Vitaliy Buturlin, Evgeny Danilovich, 2017
See the license in LICENSE
******************************************************/

#ifndef SX_AssotiativeArray_H
#define SX_AssotiativeArray_H
//#include "DSbase.h"
#include <common/MemAlloc.h>
#include <common/stack.h>
#ifdef AA_DEBUG
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#endif

#ifndef NULL
#define NULL 0
#endif

//#define SX_KEYTYPE int
//#define SX_VALTYPE int
#if defined(_WINDOWS)
#	pragma warning(push)
#	pragma warning(disable:4700)
#endif


template<typename SX_KEYTYPE, typename SX_VALTYPE, bool searchCache = false, int ReservePage = 256>
class AssotiativeArray
{
public:
	struct Node
	{
		Node * Parent;
		Node * Left;
		Node * Right;
		bool IsBlack;
		SX_KEYTYPE Key;
		SX_VALTYPE * Val;
	};
	mutable Node * TmpNode;
private:
	unsigned int Size_;
	Node * RootNode;
	
	Node* TreeSearch(const SX_KEYTYPE &key) const
	{
		if(searchCache && TmpNode && TmpNode->Key == key)
		{
			return(TmpNode);
		}
		Node * tmpCurNode = this->RootNode;

		while(tmpCurNode)
		{
			if(tmpCurNode->Key == key)
			{
				TmpNode = tmpCurNode;
				return(tmpCurNode);
			}
			tmpCurNode = tmpCurNode->Key < key ? tmpCurNode->Right : tmpCurNode->Left;
		}
		TmpNode = NULL;
		return(NULL);
	}
	void TreeRotateLeft(Node *node)
	{
		Node *tmpNode = node->Right;

		assert(tmpNode);

		node->Right = tmpNode->Left;
		if(tmpNode->Left)
		{
			tmpNode->Left->Parent = node;
		}

		tmpNode->Parent = node->Parent;

		if(node->Parent)
		{
			if(node == node->Parent->Left)
			{
				node->Parent->Left = tmpNode;
			}
			else
			{
				node->Parent->Right = tmpNode;
			}
		}
		else
		{
			this->RootNode = tmpNode;
		}

		tmpNode->Left = node;
		//if(node)
		//{
			node->Parent = tmpNode;
		//}
	}
	void TreeRotateRight(Node * node)
	{
		assert(node);
		if(!node)
		{
			return;
		}

		Node * tmpNode = node->Left;

		assert(tmpNode);
		if(!tmpNode)
		{
			return;
		}

		node->Left = tmpNode->Right;
		if(tmpNode->Right)
		{
			tmpNode->Right->Parent = node;
		}

		tmpNode->Parent = node->Parent;
		
		if(node->Parent)
		{
			if(node == node->Parent->Right)
			{
				node->Parent->Right = tmpNode;
			}
			else
			{
				node->Parent->Left = tmpNode;
			}
		}
		else
		{
			this->RootNode = tmpNode;
		}

		tmpNode->Right = node;
		
		node->Parent = tmpNode;
	}

	void TreeFixIns(Node * node)
	{
		while(node != this->RootNode && !node->Parent->IsBlack)
		{
			if(node->Parent == node->Parent->Parent->Left)
			{
				Node * tmpNode = node->Parent->Parent->Right;
				if(tmpNode && !tmpNode->IsBlack)
				{
					node->Parent->IsBlack = true;
					tmpNode->IsBlack = true;
					node->Parent->Parent->IsBlack = false;
					node = node->Parent->Parent;
				}
				else
				{
					if(node == node->Parent->Right)
					{
						node = node->Parent;
						TreeRotateLeft(node);
					}
					node->Parent->IsBlack = true;
					node->Parent->Parent->IsBlack = false;
					TreeRotateRight(node->Parent->Parent);
				}
			}
			else
			{
				Node * tmpNode = node->Parent->Parent->Left;
				if(tmpNode && !tmpNode->IsBlack)
				{
					node->Parent->IsBlack = true;
					tmpNode->IsBlack = true;
					node->Parent->Parent->IsBlack = false;
					node = node->Parent->Parent;
				}
				else
				{
					if(node == node->Parent->Left)
					{
						node = node->Parent;
						TreeRotateRight(node);
					}
					node->Parent->IsBlack = true;
					node->Parent->Parent->IsBlack = false;
					TreeRotateLeft(node->Parent->Parent);
				}
			}
		}
		this->RootNode->IsBlack = true;
	}
	void TreeFixDel(Node *node)
	{
		while(node != this->RootNode && node->IsBlack)
		{
			if(node == node->Parent->Left)
			{
				Node * tmpNode = node->Parent->Right;
				if(!tmpNode->IsBlack)
				{
					tmpNode->IsBlack = true;
					node->Parent->IsBlack = false;
					TreeRotateLeft(node->Parent);
					tmpNode = node->Parent->Right;
				}
				if((!tmpNode->Left || tmpNode->Left->IsBlack) && (!tmpNode->Right || tmpNode->Right->IsBlack))
				{
					tmpNode->IsBlack = false;
					node = node->Parent;
				}
				else
				{
					if(!tmpNode->Right || tmpNode->Right->IsBlack)
					{
						tmpNode->Left->IsBlack = true;
						tmpNode->IsBlack = false;
						TreeRotateRight(tmpNode);
						tmpNode = node->Parent->Right;
					}
					tmpNode->IsBlack = node->Parent->IsBlack;
					node->Parent->IsBlack = true;
					tmpNode->Right->IsBlack = true;
					TreeRotateLeft(node->Parent);
					node = this->RootNode;
				}
			}
			else
			{
				Node * tmpNode = node->Parent->Left;
				if(!tmpNode->IsBlack)
				{
					tmpNode->IsBlack = true;
					node->Parent->IsBlack = false;
					TreeRotateRight(node->Parent);
					tmpNode = node->Parent->Left;
				}
				if((!tmpNode->Right || tmpNode->Right->IsBlack) && (!tmpNode->Left || tmpNode->Left->IsBlack))
				{
					tmpNode->IsBlack = false;
					node = node->Parent;
				}
				else
				{
					if(!tmpNode->Left || tmpNode->Left->IsBlack)
					{
						tmpNode->Right->IsBlack = true;
						tmpNode->IsBlack = false;
						TreeRotateLeft(tmpNode);
						tmpNode = node->Parent->Left;
					}
					tmpNode->IsBlack = node->Parent->IsBlack;
					node->Parent->IsBlack = true;
					tmpNode->Left->IsBlack = true;
					TreeRotateRight(node->Parent);
					node = this->RootNode;
				}
			}
		}
		node->IsBlack = true;
	}

	template<typename... Args>
	Node* TreeInsert(const SX_KEYTYPE &key, bool *found, Args&&... args)
	{
		Node * tmpCur;
		Node * tmpParent;
		Node * tmpNode;

		tmpCur = this->RootNode;
		tmpParent = NULL;
		while(tmpCur)
		{
			if(key == tmpCur->Key)
			{
				//tmpCur->Val = val;
				//memcpy(tmpCur->Val, &val, sizeof(SX_VALTYPE));
				//*tmpNode->Val = val;
				if(found)
				{
					*found = true;
				}
				return(tmpCur);
			}
			tmpParent = tmpCur;
			tmpCur = key < tmpCur->Key ? tmpCur->Left : tmpCur->Right;
		}
		this->Size_++;
		// tmpNode = new Node;
		tmpNode = this->MemNodes.Alloc();
		assert(tmpNode);
		if(!tmpNode)
		{
			return(NULL);
		}
		tmpNode->Key = key;
		//tmpNode->Val = new SX_VALTYPE;
		tmpNode->Val = this->MemVals.Alloc(args...);
		//tmpNode->Val = AllocVal();
		//*tmpNode->Val = val;
		//memcpy(tmpNode->Val, &val, sizeof(SX_VALTYPE));
		tmpNode->Parent = tmpParent;
		tmpNode->Left = NULL;
		tmpNode->Right = NULL;
		tmpNode->IsBlack = false;

		if(tmpParent)
		{
			if(key < tmpParent->Key)
			{
				tmpParent->Left = tmpNode;
			}
			else
			{
				tmpParent->Right = tmpNode;
			}
		}
		else
		{
			this->RootNode = tmpNode;
		}

		TreeFixIns(tmpNode);
		return(tmpNode);
	}

	void SwapNodes(Node **ppRoot, Node *pA, Node *pB)
	{
		Node *pOldAParent = pA->Parent;
		Node *pOldALeft = pA->Left;
		Node *pOldARight = pA->Right;
		Node *pOldBParent = pB->Parent;
		Node *pOldBLeft = pB->Left;
		Node *pOldBRight = pB->Right;


		if(pA == *ppRoot)
		{
			*ppRoot = pB;
		}
		else if(pB == *ppRoot)
		{
			*ppRoot = pA;
		}

		if(pOldALeft)
		{
			pOldALeft->Parent = pB;
		}
		if(pOldARight)
		{
			pOldARight->Parent = pB;
		}

		if(pOldBLeft)
		{
			pOldBLeft->Parent = pA;
		}
		if(pOldBRight)
		{
			pOldBRight->Parent = pA;
		}

		if(pOldAParent && pOldAParent == pOldBParent)
		{
			std::swap(pOldAParent->Left, pOldAParent->Right);
		}
		else
		{
			if(pOldAParent)
			{
				if(pOldAParent->Left == pA)
				{
					pOldAParent->Left = pB;
				}
				else
				{
					pOldAParent->Right = pB;
				}
			}
			if(pOldBParent)
			{
				if(pOldBParent->Left == pB)
				{
					pOldBParent->Left = pA;
				}
				else
				{
					pOldBParent->Right = pA;
				}
			}
		}

		if(pA != pOldBParent)
		{
			pA->Parent = pOldBParent;
		}
		else
		{
			pA->Parent = pB;
		}
		
		if(pB != pOldAParent)
		{
			pB->Parent = pOldAParent;
		}
		else
		{
			pB->Parent = pA;
		}

		if(pA != pOldBLeft)
		{
			pA->Left = pOldBLeft;
		}
		else
		{
			pA->Left = pB;
		}

		if(pA != pOldBRight)
		{
			pA->Right = pOldBRight;
		}
		else
		{
			pA->Right = pB;
		}

		if(pB != pOldALeft)
		{
			pB->Left = pOldALeft;
		}
		else
		{
			pB->Left = pA;
		}

		if(pB != pOldARight)
		{
			pB->Right = pOldARight;
		}
		else
		{
			pB->Right = pA;
		}

		bool oldAColor = pA->IsBlack;
		pA->IsBlack = pB->IsBlack;
		pB->IsBlack = oldAColor;
	}

	void TreeDelete(const SX_KEYTYPE &key)
	{
		Node *pNode = TreeSearch(key);
		if(pNode)
		{
			this->Size_--;
			// case R2, B2
			if(pNode->Left && pNode->Right)
			{
				Node *pMinNode = minValueNode(pNode->Right);

				SwapNodes(&RootNode, pMinNode, pNode);
			}
			
			// case B1
			if(pNode->IsBlack && (pNode->Left || pNode->Right))
			{
				Node *pChildNode = pNode->Left ? pNode->Left : pNode->Right;

				pChildNode->Parent = pNode->Parent;
				if(!pNode->Parent)
				{
					RootNode = pChildNode;
				}
				else
				{
					if(pNode == pNode->Parent->Left)
					{
						pNode->Parent->Left = pChildNode;
					}
					else
					{
						pNode->Parent->Right = pChildNode;
					}
				}

				pChildNode->IsBlack = true;
				
				MemVals.Delete(pNode->Val);
				MemNodes.Delete(pNode);
				return;
			}

			// case R0, B0
			if(!pNode->Left && !pNode->Right)
			{
				Node *pParent = pNode->Parent;

				if(pParent)
				{

					Node leafNode = {0, &leafNode, &leafNode, true};

					if(pParent->Left == pNode)
					{
						pParent->Left = &leafNode;
					}
					else
					{
						pParent->Right = &leafNode;
					}
					leafNode.Parent = pParent;

					bool isBlack = pNode->IsBlack;

					MemVals.Delete(pNode->Val);
					MemNodes.Delete(pNode);

					if(isBlack)
					{
						TreeFixDel(&leafNode);
					}
					if(leafNode.Parent)
					{
						if(leafNode.Parent->Left == &leafNode)
						{
							leafNode.Parent->Left = NULL;
						}
						else
						{
							leafNode.Parent->Right = NULL;
						}
					}
					else if(RootNode == &leafNode)
					{
						RootNode = NULL;
					}
				}
				else
				{
					RootNode = NULL;
					MemVals.Delete(pNode->Val);
					MemNodes.Delete(pNode);
				}
				return;
			}
		}
	}

	Node* minValueNode(Node *node)
	{
		Node *ptr = node;

		while(ptr->Left != nullptr)
		{
			ptr = ptr->Left;
		}

		return(ptr);
	}

#ifdef AA_DEBUG
	bool TestNode(Node *pNode, int iDepth, int *pMaxDepth)
	{
		if(!pNode)
		{
			if(*pMaxDepth < 0)
			{
				*pMaxDepth = iDepth;
			}

			return(*pMaxDepth == iDepth);
		}

		if(pNode->Parent && !pNode->IsBlack && !pNode->Parent->IsBlack)
		{
			return(false);
		}

		return(TestNode(pNode->Left, iDepth + (pNode->IsBlack ? 1 : 0), pMaxDepth) && TestNode(pNode->Right, iDepth + (pNode->IsBlack ? 1 : 0), pMaxDepth));
	}

	bool TreeTest()
	{
		/*
		- Если узел красный, то оба его потомка черны.
		- На всех ветвях дерева, ведущих от его корня к листьям, число черных узлов одинаково.
		- Корень дерева всегда чёрный
		*/

		if(!RootNode)
		{
			return(true);
		}

		if(!RootNode->IsBlack)
		{
			return(false);
		}

		int iMaxDepth = -1;
		return(TestNode(RootNode, 0, &iMaxDepth));
	}

	void DrawNode(FILE *fp, Node *pNode)
	{
		static UINT uCounter = 0;
		if(!pNode)
		{
			return;
		}
		if(pNode->Parent)
		{
			fprintf(fp, "\tn%u -- n%u ;", pNode->Parent->Key, pNode->Key);
		}
		
		fprintf(fp, "\tn%u [label=\"%u\"%s] ;\n", pNode->Key, pNode->Key, pNode->IsBlack ? "" : ",color=\"#ff0000\"");

		if(pNode->Left)
		{
			DrawNode(fp, pNode->Left);
		}
		else
		{
			// uCounter
			fprintf(fp, "\tn%u -- null%u ;", pNode->Key, uCounter);
			fprintf(fp, "\tnull%u [label=\"\", shape=square, width=\".1\", height=\".1\"] ;\n", uCounter);

			++uCounter;
		}

		if(pNode->Right)
		{
			DrawNode(fp, pNode->Right);
		}
		else
		{
			// uCounter
			fprintf(fp, "\tn%u -- null%u ;", pNode->Key, uCounter);
			fprintf(fp, "\tnull%u [label=\"\", shape=square, width=\".1\", height=\".1\"] ;\n", uCounter);

			++uCounter;
		}
	}
#endif

	MemAlloc<Node, ReservePage, 8> MemNodes;
	MemAlloc<SX_VALTYPE, ReservePage, 8> MemVals;
public:

#ifdef AA_DEBUG
	void Draw()
	{
		FILE *fp = fopen("E:/src/Graphviz/inp.dot", "wb");

		fputs("graph \"\"\n{\n\tnode [fontsize=8,width=\".2\", height=\".2\", margin=0, shape=circle];\n", fp);

		DrawNode(fp, RootNode);

		fputs("}\n", fp);

		fclose(fp);

		ShellExecuteA(NULL, "open", "E:/src/Graphviz/gen.bat", "", "E:/src/Graphviz", 0);
	}
#endif

	class Iterator
	{
	private:
		const Node *m_pCurNode;

		bool IsEnd;
	public:
		const SX_KEYTYPE *first;
		SX_VALTYPE *second;

		Iterator(const Iterator &it)
		{
			this->first = it.first;
			this->second = it.second;
			this->IsEnd = it.IsEnd;
			this->m_pCurNode = it.m_pCurNode;
		}

		Iterator(const Node *node)
		{
			if(node)
			{
				const Node *tmpNode = node;
				while(tmpNode)
				{
					if(!tmpNode->Left)
					{
						this->IsEnd = false;
						this->first = &(tmpNode->Key);
						this->second = tmpNode->Val;
						this->m_pCurNode = tmpNode;
					}
					tmpNode = tmpNode->Left;
				}
			}
			else
			{
				IsEnd = true;
				this->first = NULL;
				this->second = NULL;
				this->m_pCurNode = NULL;
			}
		}

		Iterator& operator=(Iterator const &it)
		{
			this->first = it.first;
			this->second = it.second;
			this->IsEnd = it.IsEnd;
			this->m_pCurNode = it.m_pCurNode;
			return(*this);
		}

		bool operator==(const Iterator &c)
		{
			return(c.m_pCurNode == m_pCurNode);
		}

		bool operator!=(const Iterator &c)
		{
			return(c.m_pCurNode != m_pCurNode);
		}

		operator bool()
		{
			return(!this->IsEnd);
		}

		Iterator& operator++()
		{
			if(m_pCurNode)
			{
				if(m_pCurNode->Right)
				{
					// find leftmost in m_pCurNode->Right
					m_pCurNode = m_pCurNode->Right;
					while(m_pCurNode->Left)
					{
						m_pCurNode = m_pCurNode->Left;
					}

					this->first = &(m_pCurNode->Key);
					this->second = m_pCurNode->Val;
				}
				else
				{
					// find next parent
					const Node *pTempNode = m_pCurNode;
					while(pTempNode)
					{
						if(!pTempNode->Parent)
						{
							IsEnd = true;
							m_pCurNode = NULL;
							this->first = NULL;
							this->second = NULL;
						}
						else if(pTempNode->Parent->Right != pTempNode)
						{
							m_pCurNode = pTempNode->Parent;

							this->first = &(m_pCurNode->Key);
							this->second = m_pCurNode->Val;
							break;
						}

						pTempNode = pTempNode->Parent;
					}
				}
			}
			return(*this);
		}

		Iterator operator++(int)
		{
			return(++(*this));
		}
	};
	
	AssotiativeArray():RootNode(NULL), Size_(0), TmpNode(NULL)
	{
		//printf("AssotiativeArray()\n");
	}

	AssotiativeArray(const AssotiativeArray & a):RootNode(NULL), Size_(0), TmpNode(NULL)
	{
		for(typename AssotiativeArray<SX_KEYTYPE, SX_VALTYPE>::Iterator i = a.begin(); i; ++i)
		{
			(*this)[*(i.first)] = *(i.second);
		}
	}

	/*AssotiativeArray(const AssotiativeArray & a)
	{
	DS_ASSERT(false);
	this->Size_ = a.Size_;
	this->MemNodes = a.MemNodes;
	this->MemVals = a.MemVals;
	//printf("AssotiativeArray()\n");
	}*/

	bool KeyExists(const SX_KEYTYPE & key, const Node ** pNode = NULL) const
	{
		TmpNode = TreeSearch(key);
		if(pNode)
		{
			*pNode = TmpNode;
		}
		return(TmpNode != NULL);
	}

	bool KeyExists(const SX_KEYTYPE & key, const Node ** pNode = NULL, bool create = false)
	{
		bool found = !create;
		TmpNode = create ? TreeInsert(key, &found) : TreeSearch(key);
		if(pNode)
		{
			*pNode = TmpNode;
		}
		return(TmpNode != NULL && found);
	}
	template<typename... Args>
	const Node* insert(const SX_KEYTYPE &key, Args&&... args)
	{
		TmpNode = TreeInsert(key, NULL, args...);
		return(TmpNode);
	}

	AssotiativeArray & operator=(const AssotiativeArray & a)
	{
		for(typename AssotiativeArray<SX_KEYTYPE, SX_VALTYPE>::Iterator i = a.begin(); i; i++)
		{
			this->operator[](*i.first) = *i.second;
		}
		return(*this);
		/*_asm
		{
		int 3;
		};
		//DS_ASSERT(false);
		this->Size_ = a.Size_;
		this->MemNodes = a.MemNodes;
		this->MemVals = a.MemVals;*/
		//printf("AssotiativeArray()\n");
	}

	SX_VALTYPE & operator[](const SX_KEYTYPE & key)
	{
		TmpNode = TreeSearch(key);
		if(!TmpNode)
		{
			//SX_VALTYPE tmpVal = {0};
			TmpNode = TreeInsert(key, NULL);
		}
		return(*TmpNode->Val);
	}

	const SX_VALTYPE & operator[](const SX_KEYTYPE & key) const
	{
		TmpNode = TreeSearch(key);
		return(*TmpNode->Val);
	}

	const SX_VALTYPE * at(const SX_KEYTYPE & key) const
	{
		TmpNode = TreeSearch(key);
		return(TmpNode ? TmpNode->Val : NULL);
	}

//	void Insert(const SX_KEYTYPE & key, const SX_VALTYPE & val);

	unsigned int Size() const
	{
		return(this->Size_);
	}

	/*void Delete(SX_KEYTYPE key)
	{
	Node * tmpN = TreeSearch(key);
	if(tmpN)
	{
	TreeDelete(tmpN);
	}
	}*/

	void erase(const SX_KEYTYPE &key)
	{
		TreeDelete(key);
	}

	void clear()
	{
		this->RootNode = NULL;
		MemNodes.clear();
		MemVals.clear();
		this->Size_ = 0;
	}

	Iterator begin() const
	{
		return(this->RootNode);
		//Iterator i(this->RootNode);
		//return(i);
		/*Node * tmpNode = this->RootNode;
		while(tmpNode)
		{
		tmpNode = tmpNode->Left;
		if(!tmpNode->Left)
		{
		return(Iterator(tmpNode));
		}
		}
		return(Iterator(NULL));*/
	}

	/*const Node * GetRootNode()
	{
	return(RootNode);
	}*/

	Iterator end()
	{
		return(NULL);
	}

#ifdef AA_DEBUG
	bool TestIntegrity()
	{
		return(TreeTest());
	}
#endif
};

template<typename SX_KEYTYPE, typename SX_VALTYPE, bool searchCache = false, int ReservePage = 256>
using Map = AssotiativeArray<SX_KEYTYPE, SX_VALTYPE, searchCache, ReservePage>;

#if defined(_WINDOWS)
#	pragma warning(pop)
#endif

#endif
