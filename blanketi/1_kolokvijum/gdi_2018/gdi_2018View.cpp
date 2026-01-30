
// gdi_2018View.cpp : implementation of the Cgdi2018View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "gdi_2018.h"
#endif

#include "gdi_2018Doc.h"
#include "gdi_2018View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD 3.14159265 / 180.0f
#define TO_DEG 180.0f / 3.14159265

// Cgdi2018View

IMPLEMENT_DYNCREATE(Cgdi2018View, CView)

BEGIN_MESSAGE_MAP(Cgdi2018View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// Cgdi2018View construction/destruction

Cgdi2018View::Cgdi2018View() noexcept
{
	arm1 = new DImage();
	arm2 = new DImage();
	bager = new DImage();
	pozadina = new DImage();
	arm1->Load(CString("arm1.png"));
	arm2->Load(CString("arm2.png"));
	bager->Load(CString("bager.png"));
	pozadina->Load(CString("pozadina.png"));
	viljuska = GetEnhMetaFile(CString("viljuska.emf"));

	ENHMETAHEADER hdr;
	GetEnhMetaFileHeader(viljuska, sizeof(hdr), &hdr);
	viljuskaRect = CRect(
		hdr.rclBounds.left,
		hdr.rclBounds.top,
		hdr.rclBounds.right,
		hdr.rclBounds.bottom
	);

}

Cgdi2018View::~Cgdi2018View()
{
	delete arm1;
	delete arm2;
	delete bager;
	delete pozadina;
	DeleteEnhMetaFile(viljuska);	
}

BOOL Cgdi2018View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cgdi2018View drawing

void Cgdi2018View::OnDraw(CDC* pDC)
{
	// anti flicker ---------------------------
	CRect pravougaonik;
	GetClientRect(&pravougaonik);

	CDC* pMemDC = new CDC();
	pMemDC->CreateCompatibleDC(pDC);
	CBitmap bpm;
	bpm.CreateCompatibleBitmap(pDC, pravougaonik.Width(), pravougaonik.Height());
	pMemDC->SelectObject(&bpm);
	// ----------------------------------------

	XFORM staraTransformacija{};
	pMemDC->GetWorldTransform(&staraTransformacija);

	pMemDC->SetGraphicsMode(GM_ADVANCED);

	DrawBackground(pMemDC);

	Translate(pMemDC, -predjeniPut, 0);

	DrawExcavator(pMemDC);

	pMemDC->SetWorldTransform(&staraTransformacija);

	// kraj antiflicker-a ---------------------------------------------
	pDC->BitBlt(0, 0, pravougaonik.Width(), pravougaonik.Height(), pMemDC, 0, 0, SRCCOPY);
	pMemDC->DeleteDC();
	delete pMemDC;
}


// Cgdi2018View printing

BOOL Cgdi2018View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cgdi2018View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cgdi2018View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// Cgdi2018View diagnostics

#ifdef _DEBUG
void Cgdi2018View::AssertValid() const
{
	CView::AssertValid();
}

void Cgdi2018View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cgdi2018Doc* Cgdi2018View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cgdi2018Doc)));
	return (Cgdi2018Doc*)m_pDocument;
}
#endif //_DEBUG

void Cgdi2018View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM transMatrix;
	transMatrix.eM11 = sX;
	transMatrix.eM12 = 0;
	transMatrix.eM21 = 0;
	transMatrix.eM22 = sY;
	transMatrix.eDx = 0;
	transMatrix.eDy = 0;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&transMatrix, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&transMatrix, MWT_LEFTMULTIPLY);
}

void Cgdi2018View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	XFORM transMatrix;
	transMatrix.eM11 = cos(angle);
	transMatrix.eM12 = sin(angle);
	transMatrix.eM21 = -sin(angle);
	transMatrix.eM22 = cos(angle);
	transMatrix.eDx = 0;
	transMatrix.eDy = 0;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&transMatrix, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&transMatrix, MWT_LEFTMULTIPLY);
}

void Cgdi2018View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM transMatrix;
	transMatrix.eM11 = 1;
	transMatrix.eM12 = 0;
	transMatrix.eM21 = 0;
	transMatrix.eM22 = 1;
	transMatrix.eDx = dX;
	transMatrix.eDy = dY;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&transMatrix, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&transMatrix, MWT_LEFTMULTIPLY);
}

void Cgdi2018View::DrawBackground(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	int imgW = pozadina->Width();
	int imgH = pozadina->Height();

	int x = (rc.Width() - imgW) / 2;
	int y = rc.Height() - imgH;

	CRect rcImg(0, 0, imgW, imgH);              
	CRect rcDC(x, y, x + imgW, y + imgH);       

	pozadina->Draw(pDC, rcImg, rcDC);
}

void Cgdi2018View::DrawImgTransparent(CDC* pDC, DImage* pImage)
{
	int sirinaSlike = pImage->Width();
	int visinaSlike = pImage->Height();

	unsigned char* pBits = pImage->GetDIBBits();

	COLORREF bojaPrvogPiksela = RGB(pBits[2], pBits[1], pBits[0]);

	CRect izvor(0, 0, sirinaSlike, visinaSlike);
	CRect cilj(0, 0, sirinaSlike, visinaSlike);

	pImage->DrawTransparent(pDC, izvor, cilj, bojaPrvogPiksela);
}

void Cgdi2018View::DrawBody(CDC* pDC)
{
	DrawImgTransparent(pDC, bager);
	Translate(pDC, 5, 100);
}

void Cgdi2018View::DrawArm1(CDC* pDC)
{
	Translate(pDC, 58, 61);
	Rotate(pDC, ugao1 * TO_RAD);
	Translate(pDC, -58, -61);

	DrawImgTransparent(pDC, arm1);
}

void Cgdi2018View::DrawArm2(CDC* pDC)
{
	Translate(pDC, 36, 40);
	Rotate(pDC, ugao2 * TO_RAD);
	Translate(pDC, -36, -40);

	DrawImgTransparent(pDC, arm2);
}

void Cgdi2018View::DrawFork(CDC* pDC)
{
	Translate(pDC, 14, 20);
	Rotate(pDC, ugao3 * TO_RAD);
	Translate(pDC, -14, -20);
	Scale(pDC, 2.5, 2.5);


	pDC->PlayMetaFile(viljuska, viljuskaRect);
}

void Cgdi2018View::DrawExcavator(CDC* pDC)
{
	CRect prozor;
	GetClientRect(&prozor);

	Translate(pDC, prozor.Width() - bager->Width(), prozor.Height() - bager->Height());

	DrawBody(pDC);

	DrawArm1(pDC);

	Translate(pDC, 309 - 36, 61 - 40);

	DrawArm2(pDC);

	Translate(pDC, 272 - 14, 40 - 20);

	DrawFork(pDC);
	
}

// Cgdi2018View message handlers

void Cgdi2018View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int ugao = 10;
	int put = 5;

	if (nChar == '1')
	{
		if (ugao1 > -180)
			ugao1 -= ugao;
	}
	else if (nChar == '2')
	{
		if (ugao1 < -90)
			ugao1 += ugao;
	}
	else if (nChar == '3')
	{
		if (ugao2 > -170)
			ugao2 -= ugao;
	}
	else if (nChar == '4')
	{
		if (ugao2 < -90)
			ugao2 += ugao;
	}
	else if (nChar == '5')
	{
		if (ugao3 > -90)
			ugao3 -= ugao;
	}
	else if (nChar == '6')
	{
		if (ugao3 < 0)
			ugao3 += ugao;
	}
	else if (nChar == VK_LEFT)
	{
		predjeniPut += put;
	}
	else if (nChar == VK_RIGHT)
	{
		predjeniPut -= put;
	}
	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL Cgdi2018View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}
