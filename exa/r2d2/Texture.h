#pragma once

class DXTexture
{
public:
    DXTexture(): tex_(NULL) {}
    ~DXTexture() { SAFE_RELEASE(tex_); }

	void Create(UINT nWidth, UINT nHeight, D3DFORMAT d3dFormat
        , D3DPOOL d3dPool = D3DPOOL_MANAGED
        , DWORD dwUsage = 0
        );

	bool CreateSame(DXTexture& target) const;

	void Load(LPCWSTR pszFileName);

	bool Good() const { return NULL != tex_; }
	UINT GetWidth() const;
	UINT GetHeight() const;

	IDirect3DTexture9 * GetObject() const { return tex_; }

private:
	IDirect3DTexture9 * tex_;

private:
    DXTexture(DXTexture const&) {}
    DXTexture& operator = (DXTexture const&) { return *this; }
};

