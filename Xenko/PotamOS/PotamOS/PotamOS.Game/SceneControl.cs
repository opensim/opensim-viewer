using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Xenko.Core;
using Xenko.Core.Diagnostics;
using Xenko.Core.Mathematics;
using Xenko.Engine;
using Xenko.Input;
using Xenko.Physics;

namespace PotamOS
{
    public class SceneControl : SyncScript
    {
        private Task<Scene> loadingTask;
        private CancellationTokenSource loadCancellation;

        /// <summary>
        /// The loaded scene
        /// </summary>
        [DataMemberIgnore]
        public Scene Instance { get; private set; }

        [DefaultValue(false)]
        public bool ChangeScene { get; set; } = false;

        /// <summary>
        /// The url of the scene to load
        /// </summary>
        public string Url { get; set; }
        
        ///
        /// The url of the server-side currently connected to
        ///
        public string ServerUrl { get; set; }
                
        public override void Start()
        {
            base.Start();

            // Initialization of the script.
            var fileWriter = new TextWriterLogListener(new FileStream("Potamos.log", FileMode.Create));
            GlobalLogger.GlobalMessageLogged += fileWriter;

            Url = "SplashScene";
            LoadScene();
        }            

        public override void Update()
        {
            if (ChangeScene)
            {
                ToggleScene();
                Log.Info("Changing scene to " + Url + ". Current server is " + ServerUrl);
                LoadScene();
                ChangeScene = false;
            }
            
            if (Input.DownKeys.Any(e => e.Equals(Keys.Escape)) && Url == "DynamicScene") {
                ToggleScene();
                LoadScene();
            }

        }
        
        private void ToggleScene()
        {
            Entity camera = Entity.Scene.Entities.First(e => e.Name == "Camera" );
            if (Url == "SplashScene")
            {
                Url = "DynamicScene";
//                camera.Transform.Position.Z = 3.5F;
            }
            else
                Url = "SplashScene";
//                camera.Transform.Position.Z = -1F;
        }
        
        private void LoadScene()
        {
            if (Instance != null)
            {
                Content.Unload(Instance);

                // If we were the last user, detach. Ideally scripts should cooperate differently
                if (!Content.IsLoaded(Url))
                {
                    Instance.Parent = null;
                }

                Instance = null;
            }

            Instance = Content.Load<Scene>(Url);
            loadingTask = Task.FromResult(Instance);
            // Once loaded, add it to the scene
            if (Instance != null)
                Instance.Parent = Entity.Scene;

        }
    }
}
