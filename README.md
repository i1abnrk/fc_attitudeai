fc_attitudeai
=============

A dynamic AI module for freeciv
Work in progress

PURPOSE (no warranty implied)
Attitude AI is designed to mimic the behavior of SM-Civ4. The design
of which was geared toward modders like myself. SMC4 is how I learned
XML technologies, it was a great learning tool. Since freeciv uses 
flatfiles instead of xml-- Since the designer's personal feeling of 
game mechanics is that the game should produce AI that is describable 
beyond "easy", "novice", etc.-- I here attempt to produce an AI that 
is evolving and changes diplomacy co-dependantly with default AI. 
These things happen in realtime in freeciv but some consumers expect 
a certain "name" to carry a certain independant weight. This patch 
tries to remedy that idea in some small way, to make nation leaders
have more disparate goals. 

HOW IT WORKS (self-same)
I substitute an altered range to the AI "want" mechanism based on the 
leader_name in a special ruleset. The leader-nation->AI variant will 
occur iff the leader is in the "aivariant.ruleset" AND some nation 
ruleset includes the same "leader" by name insomuch. (Built-in, no 
duplicate names are allowed.) In computer science terms, the module
decreases breadth and increases rules while providing greater 
polymorphism.
Regarding that Attitude AI is a dynamically loadable shared module, 
all ruleset infos need to be loaded in modular fashion. When the 
module is not desired it will not be loaded. (This development version
does have some integration for testing purposes.) leaderrules.c|h is 
the component which loads the list of ai_variant objects from file
"aivariant.ruleset". It works with aivariant.c|h which contains all 
the constructors, destructors and member accessors for the module. In 
as much as possible, I try to mimic the freeciv code style to make it 
easy for others to implement it with their own freeciv based projects.

CURRENT STATE
Doesn't add functionality in it's current state. 
It compiles with freeciv-trunk as of writing. This may change. 
It is recommended to use only on Linux at the moment.

HOW TO BUILD
1) Build freeciv in its own directories, configured with
   --enable-shared --enable-aimodules
2) ./autogen.sh in fc_attitudeai
3) run 'configure' for fc_attitudeai, with options
   --with-freecivbld=/path/to/freeciv/builddir --with-freecivsrc=/path/to/freeciv/srcdir
4) make
