using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xenko.Core.Mathematics;
using Xenko.Input;
using Xenko.Engine;
using Xenko.Engine.Events;
using Xenko.Core.Diagnostics;
using Xenko.UI.Controls;
using Xenko.UI.Panels;
using Xenko.Core.Serialization.Contents;

namespace PotamOS
{
    public class Address : SyncScript
    {
        // Declared public member fields and properties will show in the game studio

        public override void Start()
        {
        }

        public override void Update()
        {
            // Do stuff every new frame
            if (Input.DownKeys.Any(e => e.Equals(Keys.Enter))) {
                UIPage page = Entity.Get<UIComponent>().Page;
                var overlay = page.RootElement;
                EditText abar = (EditText)overlay.FindName("AddressBar");
                Log.Info("Address is " + abar.Text);

                Entity main = Entity.Scene.Parent.Entities.First(e => e.Name == "Main");
                SceneControl controller = main.Get<SceneControl>();
                controller.HppoStr = abar.Text;
                controller.ChangeScene = true;
                
//                new EventKey<String>("InternetLocations").Broadcast(abar.Text);
//                SceneSystem.SceneInstance.RootScene = Content.Load<Scene>("DynamicScene");
            
            } 
        }
    }
}
