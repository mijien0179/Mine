#include "Draw.h"

inline HWND Mine::DrawTool::GetHwnd() const
{
	return this->hWnd;
}

LRESULT Mine::DrawTool::OnSize()
{
	RECT rc;
	GetClientRect(GetHwnd(), &rc);
	pRT->Resize(D2D1::SizeU(rc.right, rc.bottom));
	return 0;
}

LRESULT Mine::DrawTool::OnPaint(DrawSet& obj)
{
	PAINTSTRUCT ps;
	HRESULT hr;
	BeginPaint(GetHwnd(), &ps);
	pRT->BeginDraw();

	for (size_t i = 0; i < obj.GetSize(); ++i) {
		obj[i]->OnDraw(pRT, pBrush);
	}
	
	hr = pRT->EndDraw();
	EndPaint(GetHwnd(), &ps);
	return hr;
}

Mine::DrawTool::DrawTool(HWND hWnd) : hWnd{hWnd}, pRT{}, pBrush{}
{
	HRESULT hr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
	if (SUCCEEDED(hr)) {
		RECT rc;
		GetClientRect(GetHwnd(), &rc);
		hr = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(GetHwnd(), D2D1::SizeU(rc.right, rc.bottom))
			, &pRT);
	}

	if (SUCCEEDED(hr)) {
		hr = pRT->CreateSolidColorBrush(D2D1::ColorF(0.f, 0.f, 0.f), &pBrush);
	}

	if (FAILED(hr)) {
		throw "DrawTool 초기화 실패";
	}

}

Mine::DrawTool::~DrawTool()
{
	SafeRelease(pFactory);
	SafeRelease(pRT);
	SafeRelease(pBrush);
}

IDWriteTextFormat* Mine::TextTool::CreateTextFormat(const wchar_t* fontName, float fontSize)
{
	IDWriteTextFormat* pWTextFormat;
	HRESULT hr;
	hr = pWFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"ko-kr", &pWTextFormat);
	if (SUCCEEDED(hr)) {
		hr = pWTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	}
	if (SUCCEEDED(hr)) {
		hr = pWTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	return pWTextFormat;
}

Mine::TextTool::TextTool()
{
	HRESULT hr;
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pWFactory));

	if (FAILED(hr)) {
		throw "TextTool 초기화 실패";
	}

}

Mine::TextTool::~TextTool()
{
	SafeRelease(pWFactory);

}
