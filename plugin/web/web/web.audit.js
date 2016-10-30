(function() {
  "use strict";

  /*
   * Train namespace
   */
  window.Train = new Object();

  /**
   * Create the training element.
   *   @train: The trainer.
   *   @idx: The request index.
   *   &returns: The DOM element.
   */
  window.Train.elem = function(train, idx) {
    train.idx = idx;
  };
})();
