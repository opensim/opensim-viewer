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
    public class EnterButton : StartupScript
    {
        // Declared public member fields and properties will show in the game studio

        public override void Start()
        {
            UIComponent ui = Entity.Components.Get<UIComponent>();
            UIPage pg = ui.Page;
            Button enter = (Button)pg.RootElement.FindName("EnterButton");
            enter.Click += Enter_Click;

        }

        private void Enter_Click(object sender, Xenko.UI.Events.RoutedEventArgs e)
        {
            Log.Info("Mouse button clicked");

            Log.Info("Entity is " + Entity.Name);
            Log.Info("Scene is " + Entity.Scene.GetType());
            Log.Info("Parent is " + Entity.Scene.Parent.GetType());
            Log.Info("Parent has " + Entity.Scene.Parent.Entities.Count + " entities");
            foreach (var en in Entity.Scene.Parent.Entities)
                Log.Info("Entity " + en.Name);

            Entity main = Entity.Scene.Parent.Entities.First(en => en.Name == "Main");
            SceneControl controller = main.Get<SceneControl>();
            controller.SubmitForm(Entity.Scene);
        }

    }
}
