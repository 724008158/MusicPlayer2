#include "stdafx.h"
#include "PlayerUILyricsFullScreen.h"
#include "PlayListCtrl.h"


CPlayerUILyricsFullScreen::CPlayerUILyricsFullScreen(UIData& ui_data, CWnd* pMainWnd)
    : CPlayerUIBase(ui_data, pMainWnd)
{
}

CPlayerUILyricsFullScreen::~CPlayerUILyricsFullScreen()
{
}

void CPlayerUILyricsFullScreen::_DrawInfo(bool reset /*= false*/)
{
    m_draw_data.thumbnail_rect = m_draw_rect;
    CRect draw_rect = m_draw_rect;
    draw_rect.MoveToXY(0, 0);

    //����״̬��
    bool draw_status_bar = CPlayerUIHelper::IsDrawStatusBar();
    if (draw_status_bar)
    {
        CRect rc_status_bar = draw_rect;
        draw_rect.bottom -= DPI(20);
        rc_status_bar.top = draw_rect.bottom;
        DrawStatusBar(rc_status_bar, reset);
    }

    //���ƹ�����
    CRect rc_tool_bar = draw_rect;
    rc_tool_bar.top = rc_tool_bar.bottom - DPI(24) - 2 * Margin();
    rc_tool_bar.DeflateRect(Margin(), Margin());

    //����ר������
    CRect rc_album = rc_tool_bar;
    rc_album.right = rc_album.left + rc_tool_bar.Height();
    if (theApp.m_app_setting_data.show_album_cover && CPlayer::GetInstance().AlbumCoverExist())
    {
        m_draw.DrawBitmap(CPlayer::GetInstance().GetAlbumCover(), rc_album.TopLeft(), rc_album.Size(), theApp.m_app_setting_data.album_cover_fit);
    }
    else
    {
        int ICON_SIDE = DPI(16);
        int x = rc_album.left + (rc_album.Width() - ICON_SIDE) / 2;
        int y = rc_album.top + (rc_album.Height() - ICON_SIDE) / 2;
        IconRes& icon{ CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.default_cover_toolbar : theApp.m_icon_set.default_cover_toolbar_not_played };
        m_draw.DrawIcon(icon.GetIcon(false, IsDrawLargeIcon()), CPoint(x, y), CSize(ICON_SIDE, ICON_SIDE));
    }

    //���Ƹ�����Ϣ
    int info_width = rc_tool_bar.Width() / 5;
    if (info_width > DPI(200))
        info_width = DPI(200);
    CRect rc_tmp = rc_tool_bar;
    rc_tmp.left = rc_album.right + DPI(2);
    rc_tmp.right = rc_tmp.left + info_width;
    static CDrawCommon::ScrollInfo scroll_info;
    m_draw.DrawScrollText(rc_tmp, CPlayListCtrl::GetDisplayStr(CPlayer::GetInstance().GetCurrentSongInfo(), DisplayFormat::DF_ARTIST_TITLE).c_str(),
        m_colors.color_text, GetScrollTextPixel(), true, scroll_info, reset);

    //����ѭ��ģʽ��ť
    rc_tmp.left = rc_tmp.right;
    rc_tmp.right = rc_tmp.left + rc_tmp.Height();
    IconRes* pIcon = nullptr;
    switch (CPlayer::GetInstance().GetRepeatMode())
    {
    case RepeatMode::RM_PLAY_ORDER:
        pIcon = &theApp.m_icon_set.play_oder;
        break;
    case RepeatMode::RM_LOOP_PLAYLIST:
        pIcon = &theApp.m_icon_set.loop_playlist;
        break;
    case RepeatMode::RM_LOOP_TRACK:
        pIcon = &theApp.m_icon_set.loop_track;
        break;
    case RepeatMode::RM_PLAY_SHUFFLE:
        pIcon = &theApp.m_icon_set.play_shuffle;
        break;
    case RepeatMode::RM_PLAY_RANDOM:
        pIcon = &theApp.m_icon_set.play_random;
        break;
    case RepeatMode::RM_PLAY_TRACK:
        pIcon = &theApp.m_icon_set.play_track;
        break;
    }
    if (pIcon != nullptr)
        DrawControlBarBtn(rc_tmp, m_buttons[BTN_REPETEMODE], *pIcon);

    //���Ƹ������水ť
    rc_tmp.MoveToX(rc_tmp.right);
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_SKIN], theApp.m_icon_set.skin);

    //����AB�ظ���ť
    if (m_draw_rect.Width() > DPI(360))
    {
        rc_tmp.MoveToX(rc_tmp.right);
        CRect rc_btn = rc_tmp;
        rc_btn.DeflateRect(DPI(2), DPI(2));
        DrawABRepeatButton(rc_btn);
    }

    //������һ����ť
    rc_tmp.MoveToX(rc_tmp.right);
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_PREVIOUS], theApp.m_icon_set.previous_new);

    //���Ʋ���/��ͣ��ť
    rc_tmp.MoveToX(rc_tmp.right);
    const IconRes& icon_res{ CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.pause_new : theApp.m_icon_set.play_new };
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_PLAY_PAUSE], icon_res);

    //������һ����ť
    rc_tmp.MoveToX(rc_tmp.right);
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_NEXT], theApp.m_icon_set.next_new);

    //��������
    int progress_bar_left = rc_tmp.right;
    int right_max = rc_tool_bar.right;
    if (m_draw_rect.Width() > DPI(450))
    {
        rc_tmp = rc_tool_bar;
        rc_tmp.left = rc_tmp.right - DPI(72);
        DrawVolumeButton(rc_tmp);
        right_max = rc_tmp.left;
    }

    //������ʾ�����б���ť
    rc_tmp.right = rc_tmp.left + rc_tmp.Height();
    rc_tmp.MoveToX(right_max - rc_tmp.Width());
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_SHOW_PLAYLIST], theApp.m_icon_set.show_playlist);

    //���ƽ�����
    CRect rc_progress_bar = rc_tool_bar;
    rc_progress_bar.left = progress_bar_left;
    rc_progress_bar.right = rc_tmp.left;
    DrawProgressBar(rc_progress_bar);

    //��������������ť
    if (m_draw_rect.Width() > DPI(500))
        DrawVolumnAdjBtn();

    //���Ƹ��
    CRect rc_lyric = m_draw_rect;
    rc_lyric.bottom = rc_tool_bar.top;
    rc_lyric.DeflateRect(Margin(), Margin());
    m_draw_data.lyric_rect = rc_lyric;
    if (theApp.m_app_setting_data.lyric_background)
    {
        if (IsDrawBackgroundAlpha())
            m_draw.FillAlphaRect(rc_lyric, m_colors.color_lyric_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 3 / 5);
        else
            m_draw.FillRect(rc_lyric, m_colors.color_lyric_back);
    }
    rc_lyric.DeflateRect(Margin(), Margin());
    m_draw.DrawLryicCommon(rc_lyric, theApp.m_app_setting_data.lyric_align);

    //ȫ��ģʽʱ�����Ͻǻ���ʱ��
    if (m_ui_data.full_screen)
    {
        DrawCurrentTime();
    }

    //�������Ͻ�ͼ��
    DrawTopRightIcons(true);

}

int CPlayerUILyricsFullScreen::GetClassId()
{
    return 3000;
}