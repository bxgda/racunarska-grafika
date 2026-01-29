
// KLKView.cpp : implementation of the CKLKView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "KLK.h"
#endif

#include "KLKDoc.h"
#include "KLKView.h"

#include "DImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKLKView

IMPLEMENT_DYNCREATE(CKLKView, CView)

BEGIN_MESSAGE_MAP(CKLKView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CKLKView construction/destruction

CKLKView::CKLKView() noexcept
{
	m_body1 = new DImage();
	m_body1->Load(CString("images/body1.png"));

	m_arm1 = new DImage();
	m_arm1->Load(CString("images/arm1.png"));

	m_arm2 = new DImage();
	m_arm2->Load(CString("images/arm2.png"));

	m_leg1 = new DImage();
	m_leg1->Load(CString("images/leg1.png"));

	m_leg2 = new DImage();
	m_leg2->Load(CString("images/leg2.png"));

	m_background = new DImage();
	m_background->Load(CString("images/background.jpg"));
}

CKLKView::~CKLKView()
{
	delete m_body1;
	delete m_arm1;
	delete m_arm2;
	delete m_leg1;
	delete m_leg2;
	delete m_background;
}

BOOL CKLKView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CKLKView drawing

void CKLKView::Push(CDC* pDC)
{
	XFORM xf;
	pDC->GetWorldTransform(&xf);
	m_transformStack.push(xf);
}

void CKLKView::Pop(CDC* pDC)
{
	if (m_transformStack.empty())
		return;

	pDC->SetWorldTransform(&m_transformStack.top());
	m_transformStack.pop();
}

void CKLKView::DrawCoordinateSystem(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	CPen penX(PS_SOLID, 2, RGB(255, 0, 0));
	CPen penY(PS_SOLID, 2, RGB(0, 200, 0));

	CPen* oldPen = pDC->SelectObject(&penX);

	// X osa
	pDC->MoveTo(0, 0);
	pDC->LineTo(rcClient.right, 0);
	pDC->SelectObject(&penY);

	// Y osa
	pDC->MoveTo(0, 0);
	pDC->LineTo(0, rcClient.bottom);
	pDC->SelectObject(oldPen);
}

void CKLKView::DrawBackground(CDC* pDC)
{
    int imgW = m_background->Width();
    int imgH = m_background->Height();

	m_background->Draw(pDC, CRect(0, 0, imgW, imgH), CRect(0, 0, imgW, imgH));
}

void CKLKView::DrawImageTransparent(CDC* pDC, DImage* pImage)
{
	const int imgW = pImage->Width();
	const int imgH = pImage->Height();

	unsigned char* pBits = pImage->GetDIBBits();
	if (!pBits) return;
	
	COLORREF transparent = RGB(pBits[2], pBits[1], pBits[0]);

	CRect rcImg(0, 0, imgW, imgH);
	CRect rcDC(0, 0, imgW, imgH);
	
	pImage->DrawTransparent(pDC, rcImg, rcDC, transparent);
}

void CKLKView::Translate(CDC* pDC, float dx, float dy, bool rightMultiply)
{
	XFORM xf = { 1, 0, 0, 1, dx, dy };
	pDC->ModifyWorldTransform(&xf, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKLKView::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	float rad = ToRad(angle);
	XFORM xf = { cos(rad), sin(rad), -sin(rad), cos(rad), 0, 0 };
	pDC->ModifyWorldTransform(&xf, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKLKView::Scale(CDC* pDC, float sx, float sy, bool rightMultiply)
{
	XFORM xf = { sx, 0, 0, sy, 0, 0 };
	pDC->ModifyWorldTransform(&xf, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKLKView::Mirror(CDC* pDC, bool mx, bool my, bool rightMultiply)
{
	XFORM xf = {
		mx ? -1.0f : 1.0f, 0,
		0, my ? -1.0f : 1.0f,
		0, 0
	};
	pDC->ModifyWorldTransform(&xf, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKLKView::OnDraw(CDC* pDC)
{
	CKLKDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rcClient;
	GetClientRect(&rcClient);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&memBmp);

	memDC.FillSolidRect(rcClient, RGB(255, 255, 255));
	
	int oldMode =  memDC.SetGraphicsMode(GM_ADVANCED);
	XFORM oldXf;
	memDC.GetWorldTransform(&oldXf);
	
	DrawScene(&memDC);
	
	memDC.SetWorldTransform(&oldXf);
	memDC.SetGraphicsMode(oldMode);

	pDC->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBmp);
}

void CKLKView::DrawScene(CDC* pDC)
{
	DrawBackground(pDC);

	Translate(pDC, 300, m_background->Height() - 400);

	DrawTransformer(pDC);
}

void CKLKView::DrawArm1(CDC* pDC)
{
	Translate(pDC, 34, 31);
	Rotate(pDC, m_rotArm1);
	Translate(pDC, -34, -31);

	DrawImageTransparent(pDC, m_arm1);
}

void CKLKView::DrawArm2(CDC* pDC)
{
	Translate(pDC, -210+34, -102+31);
	Rotate(pDC, m_rotArm1);
	Translate(pDC, 210-34, 102-31);

	Rotate(pDC, 180 + m_rotArm2);
	DrawCoordinateSystem(pDC);
	Translate(pDC, -23, -61);
	DrawImageTransparent(pDC, m_arm2);
	Translate(pDC, 23, 61);
}

void CKLKView::DrawLeg1(CDC* pDC)
{
	Translate(pDC, 30, 125);
	Rotate(pDC, m_rotLeg1);
	Translate(pDC, -30, -125);

	DrawImageTransparent(pDC, m_leg1);
}

void CKLKView::DrawLeg2(CDC* pDC)
{
	Rotate(pDC, m_rotLeg2);
	Translate(pDC, -35, -60);

	DrawImageTransparent(pDC, m_leg2);
}

void CKLKView::DrawBody1(CDC* pDC)
{
	Translate(pDC, 26, 133);
	Rotate(pDC, m_rotBody1);
	Translate(pDC, -26, -133);

	DrawImageTransparent(pDC, m_body1);
}

void CKLKView::DrawTransformer(CDC* pDC)
{
	Push(pDC);
	{
		DrawLeg2(pDC);
	}
	Pop(pDC);
	
	Push(pDC);
	{
		Translate(pDC, -30, -125);
		DrawLeg1(pDC);

		Translate(pDC, 237, 125);
		Translate(pDC, -26, -133);
		DrawCoordinateSystem(pDC);
		DrawBody1(pDC);
		
		Translate(pDC, 212, 85);
		Translate(pDC, -34, -31);
		DrawCoordinateSystem(pDC);

		Push(pDC);
		{
			Translate(pDC, 210, 102);
			DrawArm2(pDC);
			Translate(pDC, -210, -102);
		}
		Pop(pDC);

		DrawArm1(pDC);
	}
	Pop(pDC);
}

BOOL CKLKView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

// CKLKView printing

BOOL CKLKView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKLKView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKLKView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CKLKView diagnostics

#ifdef _DEBUG
void CKLKView::AssertValid() const
{
	CView::AssertValid();
}

void CKLKView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKLKDoc* CKLKView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKLKDoc)));
	return (CKLKDoc*)m_pDocument;
}
#endif //_DEBUG


// CKLKView message handlers

void CKLKView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	float offset = 8;

	switch (nChar) {
		case 'Q':
			m_rotArm1 += offset; break;
		case 'A':
			m_rotArm1 -= offset; break;
		case 'E':
			m_rotLeg1 += offset; break;
		case 'D':
			m_rotLeg1 -= offset; break;
		case 'T':
			m_rotArm2 += offset; break;
		case 'G':
			m_rotArm2 -= offset; break;
		case 'R':
			m_rotLeg2 += offset; break;
		case 'F':
			m_rotLeg2 -= offset; break;
		case 'W':
			m_rotBody1 += offset; break;
		case 'S':
			m_rotBody1 -= offset; break;

		case VK_LEFT:
			m_rotArm1 += offset * 2.6;
			m_rotArm2 += offset * 2;
			m_rotLeg1 += offset;
			m_rotLeg2 -= offset;
			m_rotBody1 -= offset / 3;
			break;
		case VK_RIGHT:
			m_rotArm1 -= offset * 2.6;
			m_rotArm2 -= offset * 2;
			m_rotLeg1 -= offset;
			m_rotLeg2 += offset;
			m_rotBody1 += offset / 3;
			break;

		default: break;
	}

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
