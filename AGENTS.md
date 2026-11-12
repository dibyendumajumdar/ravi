# General Guidelines

* Do not commit code.
* In general, focus is on bug fixes and not enhancements.
* The process for bug fixes is as follows.
  * We first document the bug and example reproducer.
  * The Repo owner creates a github issue.
  * We then design and document the fix.
  * Finally we implement the fix and add tests.
  * The code is reviewed and committed by Repo owner.
* Lua loading of compiled chunks is accepted as unsafe and security risk - don't try to make it safe.
* Each bug or problem will be dealt with separately.
* The project has large parts derived from Lua 5.3 and Lua 5.4. Do not try to rewrite or modify parts that 
  are derived without explicit permission.