Merge Sequencer
===============

The event merging sequencer object `struct amp_merge_t` or `Merge` is used to
combine a list of sequencers into a single sequencer. Each sequencer executes
in order and given the opportunity to modify the list of events. As a result,
a subsequent sequencer will see the modifications from a previous sequencer.


## MuseLang -- `Merge`

    Merge [Seq]

The merge sequencer is available in MuseLang with the `Merge` constructor. The
`Merge` uses a list of sequencers as input and itself returns a sequencer.
