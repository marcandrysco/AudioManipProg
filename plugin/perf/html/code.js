(function() {
  "use strict";

  var refresh = function() {
    Req.get("/json", function(resp) {
      var page = Gui.byid("page");
      var json = JSON.parse(resp);

      for(var id in json) {
        var div = Gui.byid(id);
        if(div == null) {
          div = Gui.div("entry");
          div.id = id;
          page.appendChild(div);
        }

        div.innerHTML = id + ": " + json[id];
      }

      setTimeout(function() {
        refresh();
      }, 500);
    });
  };

  window.addEventListener("load", function() {
    refresh();
  });
})();
