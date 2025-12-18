// KaktusView.h : interface of the CKaktusView class
//

#pragma once
	
#include <cmath>
#include <vector>
using namespace std;

class CKaktusDoc; // forward declaration

struct CactusNode;

struct CactusBranch
{
    float angleDeg = 0.0f;         // relative angle (0 = up)
    bool light = false;            // use light metafile
    CactusNode* child = nullptr;   // child node or null for terminal
	float angleOffset = 0.0f;
};

struct CactusNode
{
    COLORREF color = RGB(0, 204, 0);
    vector<CactusBranch> branches;
};

struct LightBranchInfo
{
	CactusBranch* branch; // pointer ka originalnoj grani
	CRect buttonLeft;
	CRect buttonRight;
};

class CKaktusView : public CView
{
protected: // create from serialization only
	CKaktusView() noexcept;
	DECLARE_DYNCREATE(CKaktusView)

// Attributes
public:
	CKaktusDoc* GetDocument() const;
	
	// Grid state
	BOOL m_showGrid = TRUE;

	// Metafiles
	HENHMETAFILE m_emfPart;
	HENHMETAFILE m_emfPartLight;

    // Drawing config/state
    float m_globalRotationDeg = 180.0f; // root-based global rotation
	float m_onClickRotationDeg = 15.0f; // rotation applied on button click

	float m_rootGX = 10.0f;
	float m_rootGY = 3.0f;

    // Tree root
    CactusNode m_root;
		
	// Info about light branches and their buttons
	vector<LightBranchInfo> m_lightBranches;

	// Dynamic buttons for light branches
	CArray<CButton*, CButton*> m_lightLeftButtons;
	CArray<CButton*, CButton*> m_lightRightButtons;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate();

// Implementation
public:
	virtual ~CKaktusView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void DrawFigure(CDC* pDC);
	void DrawNode(CDC* pDC, const CactusNode& node, int branchHeight, int branchWidth, int nodeRadius);
	void DrawBranch(CDC* pDC, const CactusBranch& br, int branchHeight, int branchWidth);
	void BuildCactusTree();

	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
	void Rotate(CDC* pDC, float angleDegrees, bool rightMultiply);

	void OnLightLeftButtonClicked(UINT nID);
	void OnLightRightButtonClicked(UINT nID);
	void CollectLightBranches(CactusNode* node);

	// Cleanup helpers
	void DeleteTree(CactusNode* node);
	void ClearCactusTree();

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in KaktusView.cpp
inline CKaktusDoc* CKaktusView::GetDocument() const
   { return reinterpret_cast<CKaktusDoc*>(m_pDocument); }
#endif

