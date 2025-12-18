// KaktusView.cpp : implementation of the CKaktusView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Kaktus.h"
#endif

#include "KaktusDoc.h"
#include "KaktusView.h"
#include "KaktusHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKaktusView

IMPLEMENT_DYNCREATE(CKaktusView, CView)

BEGIN_MESSAGE_MAP(CKaktusView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_COMMAND_RANGE(1000, 1999, &CKaktusView::OnLightLeftButtonClicked)
	ON_COMMAND_RANGE(2000, 2999, &CKaktusView::OnLightRightButtonClicked)
END_MESSAGE_MAP()

void CKaktusView::OnLightLeftButtonClicked(UINT nID)
{
	int index = nID - 1000; // index dugmeta
	if (index >= 0 && index < m_lightBranches.size())
	{
		m_lightBranches[index].branch->angleOffset -= m_onClickRotationDeg;
		Invalidate(FALSE);
	}

	SetFocus();
}

void CKaktusView::OnLightRightButtonClicked(UINT nID)
{
	int index = nID - 2000;
	if (index >= 0 && index < m_lightBranches.size())
	{
		m_lightBranches[index].branch->angleOffset += m_onClickRotationDeg;
		Invalidate(FALSE);
	}

	SetFocus();
}


// CKaktusView construction/destruction

CKaktusView::CKaktusView() noexcept
{
	// TODO: add construction code here

}

CKaktusView::~CKaktusView()
{
	// Free EMFs
	if (m_emfPart)
		DeleteEnhMetaFile(m_emfPart);
	if (m_emfPartLight)
		DeleteEnhMetaFile(m_emfPartLight);

	// Free dynamically allocated cactus nodes
	ClearCactusTree();

	// Destroy dynamically created buttons
	for (int i = 0; i < m_lightLeftButtons.GetSize(); ++i) {
		CButton* btn = m_lightLeftButtons[i];
		if (btn) { btn->DestroyWindow(); delete btn; }
	}
	m_lightLeftButtons.RemoveAll();
	for (int i = 0; i < m_lightRightButtons.GetSize(); ++i) {
		CButton* btn = m_lightRightButtons[i];
		if (btn) { btn->DestroyWindow(); delete btn; }
	}
	m_lightRightButtons.RemoveAll();
}

BOOL CKaktusView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CKaktusView drawing

void CKaktusView::OnDraw(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	// 1. Kreiraj memory DC
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	// 2. Bitmap u koji crtamo
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	CBitmap* oldBmp = memDC.SelectObject(&bmp);

	// 3. Pozadina (ručno – bez WM_ERASEBKGND)
	memDC.FillSolidRect(rc, RGB(135, 206, 235));

	int oldMode = memDC.SetGraphicsMode(GM_ADVANCED);

	// 4. Clip samo na cactus oblast
	memDC.IntersectClipRect(0, 0, WINDOW_SIZE, WINDOW_SIZE);

	// 5. Crtaj sve u memDC
	DrawFigure(&memDC);

	if (m_showGrid)
		DrawGrid(&memDC, 0, 0, WINDOW_SIZE, WINDOW_SIZE, SQUARE_SIZE);

	memDC.SetGraphicsMode(oldMode);

	// 6. Jedan jedini blit na ekran
	pDC->BitBlt(
		0, 0,
		WINDOW_SIZE, WINDOW_SIZE,
		&memDC,
		0, 0,
		SRCCOPY
	);

	memDC.SelectObject(oldBmp);
}


void CKaktusView::DrawFigure(CDC* pDC)
{

	POINT rootPt = GridPoint(m_rootGX, m_rootGY);

	PushDC(pDC);
	Translate(pDC, (float)rootPt.x, (float)rootPt.y, false);
	Rotate(pDC, m_globalRotationDeg, false);

	DrawNode(pDC, m_root, SQUARE_SIZE * 2.75, SQUARE_SIZE * 2.5, NODE_RADIUS);
	DrawCircle(pDC, { 0,0 }, NODE_RADIUS);

	PopDC(pDC);

	static const vector<pair<float, float>> potOffsets = {
		{ +2.5f, +0.25f },
		{ +2.5f, +1.0f  },
		{ +2.0f, +1.0f  },
		{ +1.5f, +3.0f  },
		{ -1.5f, +3.0f  },
		{ -2.0f, +1.0f  },
		{ -2.5f, +1.0f  },
		{ -2.5f, +0.25f  }
	};

	PushDC(pDC);

	Translate(pDC, (float)rootPt.x, (float)rootPt.y, false);
	Rotate(pDC, m_globalRotationDeg, false);
	Translate(pDC, (float)-rootPt.x, (float)-rootPt.y, false);

	vector<POINT> potPoints;
	for (const auto& o : potOffsets)
	{
		potPoints.push_back(
			GridPoint(m_rootGX + o.first, m_rootGY + o.second)
		);
	}

	DrawPolygon(pDC, potPoints, RGB(222, 148, 0));
		
	// Treba se napravi helper funkcija DrawText
	CFont font;
	font.CreateFontW(
		SQUARE_SIZE * 1.2,
		0,
		0,
		0,
		FW_BOLD,
		FALSE, FALSE, FALSE,
		DEFAULT_CHARSET,
		OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		L"Arial"
	);
	CFont* oldFont = pDC->SelectObject(&font);
	pDC->SetTextColor(RGB(255, 215, 0));
	pDC->TextOutW(SQUARE_SIZE, -SQUARE_SIZE * 13, L"Biljka");
	pDC->SelectObject(oldFont);
		

	PopDC(pDC);
	
}



void CKaktusView::DrawNode(CDC* pDC, const CactusNode& node, int branchHeight, int branchWidth, int nodeRadius)
{
	// local rotation for this node affects outgoing branches
	int branchCount = (int)node.branches.size();
	float countScale = branchCount > 0 ? (1.0f / branchCount) : 1.0f; // more branches => thinner

	for (const auto& br : node.branches)
	{
		if (br.light)
		{
			LightBranchInfo info;
			info.branch = const_cast<CactusBranch*>(&br);
			m_lightBranches.push_back(info);
		}

		int rotationDegree = br.angleDeg + br.angleOffset;

		PushDC(pDC);

		// Apply node local rotation then branch angle
		Rotate(pDC, rotationDegree, false);

		// Translate forward by node radius so branch starts at circle edge (no gap)
		Translate(pDC, 0.0f, -nodeRadius / 4, false);

		// Compute width based on number of branches from this node; depth does not affect thickness
		int w = branchWidth * countScale;
		int h = branchHeight; // keep constant segment length

		// Draw branch EMF aligned upwards from edge with top-left rectangle at ( -w/2, -h ) to ( +w/2, 0 )
		Translate(pDC, -w / 2, -h, false);
		DrawBranch(pDC, br, h, w);

		// Move to end point of the branch from circle edge
		PopDC(pDC);
		PushDC(pDC);
		Rotate(pDC, rotationDegree, false);
		Translate(pDC, 0.0f, -nodeRadius / 2 - h, false);

		// Draw child node circle and recurse
		if (br.child)
		{
			DrawNode(pDC, *br.child, branchHeight, branchWidth, NODE_RADIUS);
			DrawCircle(pDC, {0,0}, NODE_RADIUS);
		}

		PopDC(pDC);
	}
}

void CKaktusView::DrawBranch(CDC* pDC, const CactusBranch& br, int branchHeight, int branchWidth)
{
	HENHMETAFILE h = br.light ? m_emfPartLight : m_emfPart;
	DrawEmfAt(pDC, h, {0,0}, branchWidth, branchHeight);
}

void CKaktusView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM x;
	x.eM11 = 1.0f; x.eM12 = 0.0f;
	x.eM21 = 0.0f; x.eM22 = 1.0f;
	x.eDx = dX;   x.eDy = dY;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&x, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&x, MWT_LEFTMULTIPLY);
}

void CKaktusView::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM x;
	x.eM11 = sX;   x.eM12 = 0.0f;
	x.eM21 = 0.0f; x.eM22 = sY;
	x.eDx = 0.0f; x.eDy = 0.0f;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&x, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&x, MWT_LEFTMULTIPLY);
}

void CKaktusView::Rotate(CDC* pDC, float angleDegrees, bool rightMultiply)
{
	double a = angleDegrees * (M_PI / 180.0);
	float c = (float)cos(a);
	float s = (float)sin(a);

	XFORM x;
	x.eM11 = c; x.eM12 = (float)s;
	x.eM21 = -(float)s; x.eM22 = c;
	x.eDx = 0.0f; x.eDy = 0.0f;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&x, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&x, MWT_LEFTMULTIPLY);
}

// CKaktusView printing

BOOL CKaktusView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKaktusView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKaktusView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CKaktusView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_emfPart = LoadEmfFromResource(IDR_CACTUS_PART);
	m_emfPartLight = LoadEmfFromResource(IDR_CACTUS_PART_LIGHT);

	BuildCactusTree();

	int btnWidth = 120;
	int btnHeight = 40;
	int x = WINDOW_SIZE + 20;
	int y = 20;

	for (size_t i = 0; i < m_lightBranches.size(); ++i)
	{
		CString leftText;
		leftText.Format(_T("LEFT %zu"), i);

		// LEFT dugme
		CButton* btnLeft = new CButton();
		btnLeft->Create(leftText, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			CRect(x, y, x + btnWidth, y + btnHeight),
			this, 1000 + (UINT)i);
		m_lightLeftButtons.Add(btnLeft);

		CString rightText;
		rightText.Format(_T("RIGHT %zu"), i);

		// RIGHT dugme
		CButton* btnRight = new CButton();
		btnRight->Create(rightText, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			CRect(x + btnWidth + 10, y, x + btnWidth * 2 + 10, y + btnHeight),
			this, 2000 + (UINT)i);
		m_lightRightButtons.Add(btnRight);

		y += btnHeight + 10;
	}
}

void CKaktusView::BuildCactusTree()
{
	auto node2 = NewNode();
	AddBranch(&m_root, 0.0f, node2);

	auto nodeLeft = NewNode();
	auto nodeRight = NewNode();
	auto nodeCenter = NewNode();

	AddBranch(node2, -45.0f);
	AddBranch(node2, 0.0f, nodeCenter);
	AddBranch(node2, +45.0f, nodeRight, true);

	AddBranch(nodeCenter, -45.0f);
	AddBranch(nodeCenter, 0.0f);
	AddBranch(nodeCenter, +45.0f);
	
	auto r2 = NewNode();
	AddBranch(nodeRight, -45.0f, nullptr);
	AddBranch(nodeRight, +45.0f, r2);

	AddBranch(r2, -45.0f, nullptr, true);
	AddBranch(r2, +45.0f);

	m_lightBranches.clear();

	CollectLightBranches(&m_root);
}

void CKaktusView::CollectLightBranches(CactusNode* node)
{
	for (auto& br : node->branches)
	{
		if (br.light)
		{
			LightBranchInfo info;
			info.branch = &br;
			m_lightBranches.push_back(info);
		}
		if (br.child)
			CollectLightBranches(br.child);
	}
}

// Recursively delete children nodes and detach pointers
void CKaktusView::DeleteTree(CactusNode* node)
{
	if (!node) return;
	for (auto& br : node->branches)
	{
		if (br.child)
		{
			DeleteTree(br.child);
			delete br.child;
			br.child = nullptr;
		}
	}
}

// Clear all dynamic nodes starting from the root
void CKaktusView::ClearCactusTree()
{
	DeleteTree(&m_root);
	m_lightBranches.clear();
}

// CKaktusView diagnostics

#ifdef _DEBUG
void CKaktusView::AssertValid() const
{
	CView::AssertValid();
}

void CKaktusView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKaktusDoc* CKaktusView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKaktusDoc)));
	return (CKaktusDoc*)m_pDocument;
}
#endif //_DEBUG

// CKaktusView message handlers

void CKaktusView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'G' || nChar == 'g')
	{
		m_showGrid = !m_showGrid;
		Invalidate(FALSE);
		return;
	}

	if (nChar == VK_LEFT)
	{
		m_lightBranches[0].branch->angleOffset -= m_onClickRotationDeg;
		Invalidate(FALSE);
		return;
	}
	if (nChar == VK_RIGHT)
	{
		m_lightBranches[0].branch->angleOffset += m_onClickRotationDeg;
		Invalidate(FALSE);
		return;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}