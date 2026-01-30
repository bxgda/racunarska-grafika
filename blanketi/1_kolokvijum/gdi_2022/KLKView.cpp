
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
	m_base= new DImage();
	m_base->Load(CString("images/base.png"));

	m_arm1 = new DImage();
	m_arm1->Load(CString("images/arm1.png"));

	m_arm2 = new DImage();
	m_arm2->Load(CString("images/arm2.png"));

	m_head = new DImage();
	m_head->Load(CString("images/head.png"));

	m_pozadina = new DImage();
	m_pozadina->Load(CString("images/pozadina.jpg"));

	m_baseShadow = new DImage();
	m_baseShadow->Load(CString("images/base_shadow.png"));

	m_arm1Shadow = new DImage();
	m_arm1Shadow->Load(CString("images/arm1_shadow.png"));

	m_arm2Shadow = new DImage();
	m_arm2Shadow->Load(CString("images/arm2_shadow.png"));

	m_headShadow = new DImage();
	m_headShadow->Load(CString("images/head_shadow.png"));
}

CKLKView::~CKLKView()
{
	delete m_base;
	delete m_arm1;
	delete m_arm2;
	delete m_head;
	delete m_pozadina;
	delete m_baseShadow;
	delete m_arm1Shadow;
	delete m_arm2Shadow;
	delete m_headShadow;
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
    CRect rc;
    GetClientRect(&rc);

    int imgW = m_pozadina->Width();
    int imgH = m_pozadina->Height();

    int x = (rc.Width() - imgW) / 2;
    int y = rc.Height() - imgH;

    CRect rcImg(0, 0, imgW, imgH);              // ceo image
    CRect rcDC(x, y, x + imgW, y + imgH);       // gde se crta

    m_pozadina->Draw(pDC, rcImg, rcDC);
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
		
	// Drzi celu lampu na dno ekrana
	CRect rc;
	GetClientRect(&rc);
	Translate(pDC, rc.Width() / 2, rc.Height() - 300);
	
	Push(pDC);
		DrawLampShadow(pDC);
	Pop(pDC);
	
	DrawLamp(pDC, false);		
}

void CKLKView::DrawLampBase(CDC* pDC, bool blsShadow)
{
	if (blsShadow)
		DrawImageTransparent(pDC, m_baseShadow);
	else
		DrawImageTransparent(pDC, m_base);
}

void CKLKView::DrawLampArm1(CDC* pDC, bool bIsShadow)
{
	Translate(pDC, 58, 61);
	Rotate(pDC, m_rotArm1);
	Translate(pDC, -58, -61);

	if (bIsShadow)
		DrawImageTransparent(pDC, m_arm1Shadow);
	else
		DrawImageTransparent(pDC, m_arm1);
}

void CKLKView::DrawLampArm2(CDC* pDC, bool bIsShadow)
{
	Rotate(pDC, m_rotArm2);
	Translate(pDC, -36, -40);

	if (bIsShadow)
		DrawImageTransparent(pDC, m_arm2Shadow);
	else
		DrawImageTransparent(pDC, m_arm2);
}

void CKLKView::DrawLampHead(CDC* pDC, bool bIsShadow)
{
	Rotate(pDC, m_rotArm2);
	
	Translate(pDC, 272 - 36, 40 - 40);
	Rotate(pDC, m_rotHead);
	Translate(pDC, -178, -100);

	if (bIsShadow)
		DrawImageTransparent(pDC, m_headShadow);
	else
		DrawImageTransparent(pDC, m_head);
}

void CKLKView::DrawLamp(CDC* pDC, bool bIsShadow)
{
	DrawLampBase(pDC, bIsShadow);
	Translate(pDC, m_base->Width() / 2 - 30, -50);	// ovo vrv fali u tekstu zadatka
	DrawLampArm1(pDC, bIsShadow);

	Translate(pDC, 309, 61);

	Push(pDC);
		DrawLampHead(pDC, bIsShadow);
	Pop(pDC);
	
	DrawLampArm2(pDC, bIsShadow);
}

void CKLKView::DrawLampShadow(CDC* pDC)
{
	DrawCoordinateSystem(pDC);
	Translate(pDC, m_base->Width() / 2 - 40, m_base->Height() / 2 + 40);

	Scale(pDC, 1, 0.25);
	Rotate(pDC, -90);

	DrawCoordinateSystem(pDC);
	DrawLamp(pDC, true);
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
	int offset = 10;

	switch (nChar) {
		case '1':
			m_rotArm1 -= offset;
			break;
		case '2':
			m_rotArm1 += offset;
			break;
		case '3':
			m_rotArm2 -= offset;
			break;
		case '4':
			m_rotArm2 += offset;
			break;
		case '5':
			m_rotHead -= offset;
			break;
		case '6':
			m_rotHead += offset;
			break;
	}

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}