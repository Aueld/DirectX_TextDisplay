
// TextDisplayView.cpp: CTextDisplayView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "TextDisplay.h"
#endif

#include "TextDisplayDoc.h"
#include "TextDisplayView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTextDisplayView

IMPLEMENT_DYNCREATE(CTextDisplayView, CView)

BEGIN_MESSAGE_MAP(CTextDisplayView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTextDisplayView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CTextDisplayView 생성/소멸

CTextDisplayView::CTextDisplayView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CTextDisplayView::~CTextDisplayView()
{
}

BOOL CTextDisplayView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CTextDisplayView 그리기

void CTextDisplayView::OnDraw(CDC* /*pDC*/)
{
	CTextDisplayDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CTextDisplayView 인쇄


void CTextDisplayView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CTextDisplayView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CTextDisplayView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CTextDisplayView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CTextDisplayView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CTextDisplayView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CTextDisplayView 진단

#ifdef _DEBUG
void CTextDisplayView::AssertValid() const
{
	CView::AssertValid();
}

void CTextDisplayView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTextDisplayDoc* CTextDisplayView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTextDisplayDoc)));
	return (CTextDisplayDoc*)m_pDocument;
}
#endif //_DEBUG


// CTextDisplayView 메시지 처리기
void CTextDisplayView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

BOOL CTextDisplayView::CreateSurfaces(void)
{
	// 변수선언 및 초기화
	DDSURFACEDESC2 ddSD;
	DDSCAPS2 ddsC;
	BOOL surface = FALSE;
	HRESULT result;

	// DDSUPFACEDESC 구조 초기화
	memset(&ddSD, 0, sizeof(DDSURFACEDESC2));
	ddSD.dwSize = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddSD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddSD.dwBackBufferCount = 1;

	//Primary suface 와 back buffer
	result = m_pDDObj->CreateSurface(&ddSD, &m_pPS, NULL);

	if (result == DD_OK) {
		ZeroMemory(&ddsC, sizeof(ddsC));
		ddsC.dwCaps = DDSCAPS_BACKBUFFER;
		result = m_pPS->GetAttachedSurface(&ddsC, &m_pBB);
		if (result == DD_OK)
			surface = TRUE;
	}
	return surface;
}

BOOL CTextDisplayView::ClearSurface(LPDIRECTDRAWSURFACE7 pSurface)
{
	//변수 선언 및 초기화
	DDSURFACEDESC2 ddSD;
	BOOL surface = FALSE;
	BOOL exitDo = FALSE;
	HRESULT result;

	// 지울 surface의 메모리 영역 잠금
	memset(&ddSD, 0, sizeof(DDSURFACEDESC2));
	ddSD.dwSize = sizeof(DDSURFACEDESC2);

	do {
		result = pSurface->Lock(NULL, &ddSD, DDLOCK_SURFACEMEMORYPTR, NULL);

		if (result == DDERR_SURFACELOST)
			m_pPS->Restore();
		else if (result != DDERR_WASSTILLDRAWING)
			exitDo = TRUE;
	} while (!exitDo);


	// 잠근 면 삭제 및 unlock() 함수 이용 잠금 해제
	if (result == DD_OK) {
		UINT S_width, S_height;

		S_width = ddSD.lPitch;
		S_height = ddSD.dwHeight;

		char* buf = (char*)ddSD.lpSurface;
		memset(buf, 0, S_width * S_height);

		pSurface->Unlock(NULL);
		surface = TRUE;

	}
	return TRUE;
}

void CTextDisplayView::UpdateSurface(void)
{
	static BOOL phase = TRUE;
	HDC hdc;
	DDBLTFX ddbltfx;		// 화면 배경 색상 지정
	RECT rc;			// 화면 전체의 ClientRect 획득
	SIZE size;


	// 첫 번째와 두 번째 화면 생성
	if (phase) {
		ddbltfx.dwSize = sizeof(ddbltfx);
		ddbltfx.dwFillColor = 0;

		m_pBB->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
		if (m_pBB->GetDC(&hdc) == DD_OK) {
			SetBkColor(hdc, RGB(0, 0, 0));			// 배경 색상 검정
			SetTextColor(hdc, RGB(255, 255, 255));	// 문자 색상 흰색
			TextOut(hdc, 0, 0, m_FrontMsg, lstrlen(m_FrontMsg));

			phase = FALSE;
		}
	}
	else {
		ddbltfx.dwSize = sizeof(ddbltfx);
		m_pBB->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
		if (m_pBB->GetDC(&hdc) == DD_OK) {
			SetBkColor(hdc, RGB(255, 255, 255));
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, 0, 0, m_BackMsg, lstrlen(m_BackMsg));

			phase = FALSE;
		}
	}

	// 화면 중앙에 문장 표시
	GetClientRect(&rc);
	
	GetTextExtentPoint(hdc, m_TitleMsg, lstrlen(m_TitleMsg), &size);
	TextOut(hdc, (rc.right - size.cx) / 2, (rc.bottom - size.cy) / 2, m_TitleMsg, lstrlen(m_TitleMsg));

	GetTextExtentPoint(hdc, m_PressMsg, lstrlen(m_PressMsg), &size);
	TextOut(hdc, (rc.right - size.cx) / 2, (rc.bottom - size.cy + 40) / 2, m_PressMsg, lstrlen(m_PressMsg));

	GetTextExtentPoint(hdc,m_EndMsg, lstrlen(m_EndMsg), &size);
	TextOut(hdc, (rc.right - size.cx) / 2, (rc.bottom - size.cy + 80) / 2, m_EndMsg, lstrlen(m_EndMsg));

	m_pBB->ReleaseDC(hdc);
}

void CTextDisplayView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((nChar == VK_SPACE) && (m_DDrawOK)) {
		HRESULT result;
		BOOL exitDo = FALSE;

		UpdateSurface();
		do {
			result = m_pPS->Flip(NULL, DDFLIP_WAIT);

			if (result == DDERR_SURFACELOST)
				m_pPS->Restore();
			else if(result != DDERR_WASSTILLDRAWING)
				exitDo = TRUE;

		} while(!exitDo);
	}
	else if (nChar == VK_ESCAPE) {
		GetParentFrame()->PostMessage(WM_CLOSE);
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTextDisplayView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	HRESULT result;
	BOOL surface;
	m_DDrawOK = FALSE;
	m_pPS = NULL;
	m_pBB = NULL;

	// 화면에 표시될 문장들
	m_TitleMsg = "두 개의 화면이 Flip되는 프로그램";
	m_PressMsg = "Space Bar 입력시 화면 전환";
	m_FrontMsg = "Primary Buffer";
	m_BackMsg = "Back Buffer";
	m_EndMsg = "ESC Key를 누르면 프로그램 종료";

	// DirectDraw 객체 생성
	result = DirectDrawCreateEx(NULL, (LPVOID*)&m_pDDObj, IID_IDirectDraw7, NULL);

	if (result != DD_OK)
		return;

	HWND hWnd = AfxGetMainWnd()->m_hWnd;
	result = m_pDDObj->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);

	if (result != DD_OK)
		return;

	result = m_pDDObj->SetDisplayMode(1280, 720, 8, 0, 0);

	if (result != DD_OK)
		return;

	surface = CreateSurfaces();
	if (!surface)
		return;

	m_DDrawOK = ClearSurface(m_pBB);
	UpdateSurface();
	BOOL exitDo = FALSE;

	do {
		result = m_pPS->Flip(NULL, DDFLIP_WAIT);
		if (result == DDERR_SURFACELOST)
			m_pPS->Restore();
		else if (result != DDERR_WASSTILLDRAWING)
			exitDo = TRUE;
	} while (!exitDo);

}