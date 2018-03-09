# Contributing to SolarPILOT

You can contribute to SolarPILOT by letting us know about problems or suggesting new features, or by making your own changes or additions to the code. You may want to help us fix an [issue someone else reported](https://github.com/NREL/SolarPILOT/issues), fix an issue you discovered, or add a new feature to SolarPILOT.

## Let us know about a problem or suggest a new feature

If you find a problem with SolarPILOT, or would like to request a new feature, let us know by [submitting an issue](https://github.com/NREL/SolarPILOT/issues/new).

If you have a question about using SolarPILOT, you can ask us at [SolarPILOT support](mailto://solarpilot.support@nrel.gov).

## Contribute code to the SolarPILOT project

If you are ready to contribute code to SolarPILOT, there are a couple of things you should know first:

* First off, please read [SolarPILOT's Contribution Policy](https://github.com/NREL/SolarPILOT/wiki/Contribution-Policy).  In particular, you'll need to send us an email (to [solarpilot.support@nrel.gov](mailto://solarpilot.support@nrel.gov)) that states:

_I agree to contribute to SolarPILOT. I agree to the following terms and conditions for my contributions: First, I agree that I am licensing my contributions under the terms of the current SolarPILOT license. Second, I agree that, in order to conform to any future open source software license(s) under which SolarPILOT may be provided, the terms of my license may be modified without any notice to me and without my consent. Third, I represent and warrant that I am authorized to make the contributions and grant the license. If my employer has rights to intellectual property that includes my contributions, I represent and warrant that I have received permission to make contributions and grant the required license on behalf of my employer._

* SolarPILOT consists of [several code repositories](https://github.com/NREL/SolarPILOT/wiki/Software-Dependencies), so you will need to determine where to make your contribution. For example, if you are making a change to the way SolarPILOT's heliostat positioning algorithm model makes a calculation, you would work with the [SSC](https://github.com/NREL/SSC) repository. If you are adding a new feature that changes both calculations and the user interface, like adding a new layout algorithm, then you would work with both the SolarPILOT repository and the SSC repository. If you need help figuring out where your contribution should go, please [let us know](mailto://solarpilot.support@nrel.gov).

* We use GitHub to manage the open source project, so you will need to learn how to use it to fork, clone, branch, check out, pull, add, commit, and push your work.

### Instructions

Here are the steps that will prepare you to contribute code to SolarPILOT:

1. Install GitHub on your computer.
1. Follow the instructions on the [SolarPILOT wiki](https://github.com/NREL/SolarPILOT/wiki) to clone the SolarPILOT repositories and build SolarPILOT.
1. Create a fork on GitHub.com for the repository (SolarPILOT, SolTrace, SSC, LK, or WEX) you are contributing to.
1. Clone your fork to create a local copy on your computer.
1. Follow the best practices steps to contribute your code.

### Best practices

The Git VCS is most effective when new development occurs in a branch and the proposed code changes are submitted via a _pull request_ into one of the main branches. The repository is configured to require special steps to be taken when contributing to one of the protected branches. This helps maintain stable releases and ensure new content conforms with the goals of the project. 

The repository is organized into several branches, and some have special importance. 

<table>
<tr><th>Branch</th><th>Access</th><th>Description</th></tr>
<tr><td>master</td><td>admin</td><td>Current stable version. This branch can only be modified by admins.</td></tr>
<tr><td>develop</td><td>SolarPILOT-RW</td><td>Development version. Submit new features or non-patch fixes here via pull request.</td></tr>
<tr><td>version-x.y</td><td>SolarPILOT-RW</td><td>Historical release version. This branch can only be modified by admins.</td></tr>
<tr><td>_other_</td><td>Contributors</td><td>All new development occurs in named branches. These will be merged into other topic-specific branches or into develop.</td></tr>
</table>

The following steps should be taken when contributing code.

1. Determine the branch to which your code should apply.
    * Bug fixes for the existing release should start from the _version-x.y_ branch. 
    * Bug fixes that don't affect the current release should start from _develop_.
    * New features should start from _develop_ or a sub-branch.

2. If you have a bug fix, create an Issue on the issue tracker. If you are fixing a bug on the _develop_ branch and want it applied to prior stable releases as a patch, label the issue with ```needs-patch```.
3. ```checkout``` the appropriate branch. Pull to the Head. 
4. Create and checkout a new branch for your changes. ```git checkout -b <new branch name>```.
5. Make your changes and commit them to your branch with frequent, descriptive commits.
6. Build SolarPILOT and test it to make sure your code works as expected (see [below](#test-protocol)).
7. ```git push origin <new branch name>``` to publish your branch. 
8. On the [repository](https://github.com/NREL/SolarPILOT) web page, create a pull request for your new branch. 
    * Specify the target branch according to the selection from step (1) above.
    * Make sure to fully comment your pull request. 
    * The project admins will review your pull request and solicit discussion if any issues arise. If your pull request is accepted, the branch will be deleted, and you can delete your local copy of the branch. If the request is not accepted, please review the discussion to determine and resolve any issues.

### Resources for Learning GitHub

If you are new to GitHub, you can find helpful articles to help you learn how it works on the web. Some examples are:

* [Using the Fork-and-Branch Git Workflow](https://blog.scottlowe.org/2015/01/27/using-fork-branch-git-workflow/) from Scott's Weblog is a good introduction.

* [Git Concepts: Branches, Forks, and Pull Requests](http://willi.am/blog/2014/05/12/git-concepts-branches-forks-and-pull-requests/) from Will Anderson is useful, although the video on the page does not work.

* [3.2 Git Branching - Basic Branching and Merging](https://www.git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging) from the Git documentation.

* [Fork a Repo](https://help.github.com/articles/fork-a-repo/) from GitHub Help.

* [About pull requests](https://help.github.com/articles/about-pull-requests/) from GitHub Help.

### Test Protocol

We are in the process of setting up a Google Test framework for testing your contribution to ensure that it does not cause any problems with the software.

For now, you can help to ensure that your code works with the rest of SolarPILOT by:

1. Compiling SolarPILOT with your contribution for Windows, Mac, and Linux.

2. Fixing any compiler warning messages.

3. Running the compiled program with several configurations.

For example, if you made a change to how the heliostat layout algorithm works, you should test layout, flux simulation, and optimization runs with different heliostat configurations, numbers of heliostat templates, and spacing requirements.

**Note that a change to SSC may affect performance in projects that also use SSC, such as SDKTool and SAM. Changes to the code should be tested within each project that uses the modified library.** Please refer to the list of [project dependencies](https://github.com/NREL/SolarPILOT/wiki/Software-Dependencies) for more information.
