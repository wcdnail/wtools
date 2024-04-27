namespace Twins
{
    enum
    {
        HeaderHeight        = 19,
        PaintAreLocked      = 1,
        PaintAreUnlocked    = 0,
    };

    //struct AutoLocker
    //{
    //    AutoLocker(PanelView const& view, bool linvalidate, bool uinvalidate)
    //        : VeiwRef(view)
    //        , Refresh(uinvalidate)
    //    {
    //        VeiwRef.Lock(linvalidate);
    //    }
    //
    //    ~AutoLocker()
    //    {
    //        VeiwRef.Unlock(Refresh);
    //    }
    //
    //private:
    //    PanelView const& VeiwRef;
    //    bool Refresh;
    //};

    struct BufferedPaint
    {
        BufferedPaint(CDC& paintDc, CRect const& rcClient, CRect const& rcPaint, CDC& SnapshotDc, HBITMAP& SnapshotBm)
            : DestDcRef(paintDc)
            , SnapshotDcRef(SnapshotDc)
            , rcPaintRef(rcClient)
        {
            CDC tempDc(::CreateCompatibleDC(paintDc));
            CBitmap tempBmp(::CreateCompatibleBitmap(paintDc, rcClient.Width(), rcClient.Height()));
            HBITMAP oldBm = tempDc.SelectBitmap(tempBmp);

            if ((NULL != SnapshotDc.m_hDC && NULL != SnapshotBm))
                SnapshotDc.SelectBitmap(SnapshotBm);

            SnapshotDc.Attach(tempDc.Detach());
            SnapshotBm = oldBm;
        }

        ~BufferedPaint()
        {
            DestDcRef.BitBlt(rcPaintRef.left, rcPaintRef.top, rcPaintRef.Width(), rcPaintRef.Height(), SnapshotDcRef, rcPaintRef.left, rcPaintRef.top, SRCCOPY);
        }

    private:
        CDC& DestDcRef;
        CDC& SnapshotDcRef;
        CRect const& rcPaintRef;
    };
}
