modded class LoadingScreen
{
	static const int LOADING_SCREEN_COUNT = 12;
	
	protected bool m_EditorImagePicked;
	
	void LoadingScreen(DayZGame game)
	{		
		m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets("DayZEditor/GUI/layouts/EditorLoadingScreen.layout");
		
		/*m_ImageLogoMid.LoadImageFile(0, "DayZEditor/gui/images/logo_editor_big.edds");
		m_ImageLogoMid.SetImage(0);
		m_ImageLogoMid.SetFlags(WidgetFlags.SOURCEALPHA | WidgetFlags.BLEND);

		m_ImageLogoCorner.LoadImageFile(0, "DayZEditor/gui/images/logo_editor_big.edds");
		m_ImageLogoCorner.SetImage(0);
		m_ImageLogoCorner.SetFlags(WidgetFlags.SOURCEALPHA | WidgetFlags.BLEND);
		
		m_ImageLogoMid.Show(true);*/
		
		Class.CastTo(m_ImageLogoMid, m_WidgetRoot.FindAnyWidget("ImageLogoMid"));
		Class.CastTo(m_ImageLogoCorner, m_WidgetRoot.FindAnyWidget("ImageLogoCorner"));
		
		Class.CastTo(m_TextWidgetTitle, m_WidgetRoot.FindAnyWidget("TextWidget"));
		Class.CastTo(m_TextWidgetStatus, m_WidgetRoot.FindAnyWidget("StatusText"));
		Class.CastTo(m_ImageWidgetBackground, m_WidgetRoot.FindAnyWidget("ImageBackground"));
		Class.CastTo(m_ImageLoadingIcon, m_WidgetRoot.FindAnyWidget("ImageLoadingIcon"));
		Class.CastTo(m_ModdedWarning, m_WidgetRoot.FindAnyWidget("ModdedWarning"));
		
		m_ImageBackground = ImageWidget.Cast( m_WidgetRoot.FindAnyWidget("ImageBackground") );
		m_ProgressLoading = ProgressBarWidget.Cast( m_WidgetRoot.FindAnyWidget("LoadingBar") );
		
		int screen_x, screen_y;
		GetScreenSize(screen_x, screen_y);
		//m_ImageLogoMid.SetPos();
		
		//m_ImageLogoCorner.Show(true);
		
		m_ImageWidgetBackground.SetFlags(WidgetFlags.SOURCEALPHA | WidgetFlags.BLEND);
		//m_EditorLogo = game.GetLoadingWorkspace().CreateWidgets("DayZEditor/gui/layouts/EditorLogo.layout", m_ImageWidgetBackground);
				
		m_ModdedWarning.SetText("WARNING! The DayZ Editor is currently in BETA. Please report all bugs you find to our Discord");	
		m_ModdedWarning.Show(true);
		
	}
	
	
	override void Show()
	{
		Widget lIcon = m_ImageWidgetBackground;
		Widget pText = m_ProgressLoading;
		m_ProgressText.SetText("");
		m_ProgressLoading.SetCurrent( 0.0 );
		m_ImageWidgetBackground.SetMaskProgress( 0.0 );
		
		if ( !m_WidgetRoot.IsVisible() )
		{
			if ( m_DayZGame.GetUIManager().IsDialogVisible() )
			{
				m_DayZGame.GetUIManager().HideDialog();
			}
			
			if ( m_DayZGame.GetMissionState() == DayZGame.MISSION_STATE_MAINMENU )
			{
				//m_ProgressBar = m_ProgressBarSmall;
				m_ImageLogoMid.Show(false);
				m_ImageLogoCorner.Show(false);				
				//m_FrameWidgetSmall.Show( true );
				//m_FrameWidgetLarge.Show( false );
				m_ImageWidgetBackground.Show( true );		
				m_TextWidgetStatus.Show(true);
				//lIcon = null;
				//pText = null;
			}
			else
			{
				//m_ProgressBar = m_ProgressBarLarge;
				m_ImageLogoMid.Show(true);
				m_ImageLogoCorner.Show(false);
				//m_FrameWidgetSmall.Show( false );
				//m_FrameWidgetLarge.Show( true );
				m_ImageWidgetBackground.Show( true );
				m_TextWidgetStatus.Show(false);
			}
			
			m_WidgetRoot.Show( true );
			//m_ProgressBar.SetCurrent( 0 );
			m_TextWidgetTitle.SetText( "" );
			m_TextWidgetStatus.SetText( "" );
		}
		
		ProgressAsync.SetProgressData(pText);
		ProgressAsync.SetUserData(lIcon);
		
		// this solution is hacky, but Math.Random doesnt work in the constructor of this
		if (!m_EditorImagePicked) {
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(_LoadRandomImage, 0);
		}
	}
	
	private void _LoadRandomImage()
	{
		m_ImageWidgetBackground.LoadImageFile(0, string.Format("DayZEditor/gui/loadingscreens/%1.edds", Math.RandomIntInclusive(0, LOADING_SCREEN_COUNT)));
		m_ImageWidgetBackground.SetImage(0);
		
		// e a s t e r e g g
		if (Math.RandomInt(0, 100) == 69)
			m_ModdedWarning.SetText("You are running a modded version of the game. If shits fucked? Deal with it.");
		
		m_EditorImagePicked = true;
	}
}