Term Merge
------------------------
by:
 * Max Nylin <max@articstudios.se>
 * Oleksandr Trotsenko

Description
-----------
When using taxonomy for free tagging purposes, it's easy to end up with
several terms having the same meaning. This may be due to spelling errors,
or different users simply making up synonymous terms as they go.

You, as an administrator, may then want to correct such errors or unify
synonymous terms, thereby pruning the taxonomy to a more manageable set.
This module allows you to merge multiple terms into one, while updating
all fields referring to those terms to refer to the replacement term instead.

Currently, the module only acts on fields. It would be desirable to update
other possible places where deleted terms are used.

Integration
-------------
Currently module integrates with the following core and contributed modules:
 * Redirect module (http://drupal.org/project/redirect). During term merging
 you may set up SEO friendly redirects from the branch terms to point to the
 trunk term

Requirements
-------------
The modules requires enabled the following modules:
 * Taxonomy module (ships with Drupal core)
 * Entity API (http://drupal.org/project/entity)

Installation
------------
 * Copy the module's directory to your modules directory and activate the
 module.
