using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xenko.Core.Mathematics;
using Xenko.Input;
using Xenko.Engine;
using Xenko.Games;
using Xenko.UI.Panels;
using Xenko.UI.Controls;
using Xenko.UI;

namespace PotamOS
{
    /// <summary>
    /// This script is supposed to be attached to the Main Entity in the Main scene
    /// </summary>
    public class UIControlScript : SyncScript
    {
        // Declared public member fields and properties will show in the game studio
        protected Game PotamOS;

        protected bool IsRunning;

        protected bool UICreated;

        #region Visuals

        private UIPage m_Page;

        private StackPanel m_AddressPanel; 
        private ModalElement m_LoginGrid;

        /// <summary>
        /// UI library 
        /// </summary>
        public UILibrary UILibrary { get; set; }

        #endregion Visuals

        private SceneControl m_SceneController;
        
        public override void Start()
        {
            IsRunning = true;

            Log.Info("UIControlScript starting");
            //PotamOS = (Game)Services.GetServiceAs<IGame>();

            //AdjustVirtualResolution(this, EventArgs.Empty);
            //Game.Window.ClientSizeChanged += AdjustVirtualResolution;

            m_SceneController = Entity.Get<SceneControl>();
            
            CreateUI();
        }

        public override void Update()
        {
            // Do stuff every new frame
            if (m_SceneController.Url == "SplashScene" && Input.DownKeys.Any(e => e.Equals(Keys.Enter)))
            {
                EditText addressEditText = m_AddressPanel.FindVisualChildOfType<EditText>("Address");

                Log.Info("Address is " + addressEditText.Text);

                m_SceneController.Goto(addressEditText.Text);
            }
        }

        public override void Cancel()
        {
            Game.Window.ClientSizeChanged -= AdjustVirtualResolution;

            IsRunning = false;
            UICreated = false;
        }

        private void AdjustVirtualResolution(object sender, EventArgs e)
        {
            var backBufferSize = new Vector2(GraphicsDevice.Presenter.BackBuffer.Width, GraphicsDevice.Presenter.BackBuffer.Height);
            Entity.Get<UIComponent>().Resolution = new Vector3(backBufferSize, 1000);
        }

        protected void CreateUI()
        {
            if (!UICreated)
                LoadUI();

            UICreated = true;
        }

        protected void LoadUI()
        {
            // Allow user to resize the window with the mouse.
            //Game.Window.AllowUserResizing = true;

            // Initialize UI
            m_Page = Entity.Get<UIComponent>().Page;
            InitializeAddressPanel();
            InitializeLoginGrid();
            CloseLoginGrid();
            // Add the two to the overlay
            var overlay = (Grid)m_Page.RootElement;
            overlay.Children.Add(m_AddressPanel);
            overlay.Children.Add(m_LoginGrid);
        }

        private void InitializeAddressPanel()
        {            
            m_AddressPanel = UILibrary.InstantiateElement<StackPanel>("AddressBarPanel");
            //EditText addressEditText = m_AddressPanel.FindVisualChildOfType<EditText>("Address");
            //addressEditText.TextChanged += delegate
            //{
            //    Log.Info("Text Changed");
            //    if (Input.Keyboard.DownKeys.Any(k => k.Equals(Keys.Enter)))
            //    {
            //        EditText abar = (EditText)m_AddressPanel.FindName("Address");
            //        Log.Info("Address is " + abar.Text);
            //        m_SceneController.Goto(abar.Text);
            //        CloseAddressPanel();
            //    }
            //};
        }
        
        private void InitializeLoginGrid()
        {
            m_LoginGrid = UILibrary.InstantiateElement<ModalElement>("LoginPopup");
            Button enter = m_LoginGrid.FindVisualChildOfType<Button>("Enter");
            enter.Click += delegate
            {
                Log.Info("Mouse button clicked");
                EditText name = m_LoginGrid.FindVisualChildOfType<EditText>("Name");
                m_SceneController.SubmitForm(name.Text);

                CloseLoginGrid();
            };
        }

        public void ShowAddressPanel()
        {
            m_AddressPanel.Visibility = Visibility.Visible;
        }

        public void ShowLoginGrid(string destination)
        {
            TextBlock dest = m_LoginGrid.FindVisualChildOfType<TextBlock>("Destination");
            dest.Text = string.Format("Destination: {0}", destination);
            m_LoginGrid.Visibility = Visibility.Visible;
        }

        public void CloseAddressPanel()
        {
            m_AddressPanel.Visibility = Visibility.Collapsed;
        }

        public void CloseLoginGrid()
        {
            m_LoginGrid.Visibility = Visibility.Collapsed;
        }
    }
}
