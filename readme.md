# Preserving Executable History

> **Reconstructing the source and infrastructure required to keep historically significant software alive after its original technological environment has disappeared.**

## Mission

Video games are not preserved merely because a disc image, executable, screenshot, or video survives.

Interactive software depends on an entire technological environment: operating systems, processors, graphics APIs, authentication services, network infrastructure, middleware, timing behavior, peripherals, and implementation knowledge. When those dependencies disappear, a historically important game can remain copyrighted for many decades while becoming impossible to experience in anything close to its original form.

This project exists to preserve **executable history**.

The goal is to reconstruct historically significant software whose original execution environment has disappeared, document how that reconstruction was performed, preserve the implementation knowledge required to maintain it, and make it possible for future historians, programmers, researchers, and players to understand and experience these works without having to rediscover everything from opaque binaries generations after the people who lived through the original era are gone.

This work is not intended to replace, compete with, or interfere with currently sold games, current authentication systems, current online services, or current commercial networks.

---

## Why Source Code Matters to Preservation

Preserving only a binary solves the problem temporarily.

A reconstructed Windows executable produced today may itself become difficult or impossible to run in the future. Processor architectures, operating systems, graphics APIs, compilers, security models, and development tools will continue to change.

If only another opaque executable survives, future preservationists may eventually face the same problem again:

```text
1997 original executable
        ↓
original environment disappears
        ↓
2026 preservation executable
        ↓
2026 environment eventually disappears
        ↓
future historians must reverse engineer everything again
```

Preserving source changes that:

```text
original historical artifact
        +
reconstructed source
        +
documentation
        +
behavioral tests
        +
build information
        +
preservation infrastructure
        ↓
future historians can port and validate the implementation
```

Source code is therefore not merely a convenience. In a preservation context, it can be part of the historical record itself.

People working on preservation today also possess cultural and technical context that may disappear with them. We remember how these systems behaved, what seemed normal at the time, which quirks were authentic, what latency and timing felt like, what hardware assumptions developers were making, and which behaviors distinguish the historical software from a modern reinterpretation.

A researcher a century from now may have videos and screenshots but lack that frame of reference.

For that reason this project attempts to preserve not only code, but also **reconstruction decisions, known ambiguities, behavioral expectations, historical context, and validation material**.

---

# The 1997 StarCraft Beta

This work concerns an **externally distributed 1997 StarCraft multiplayer beta** originally provided on CD-ROM.

The original physical disc remains in the possession of the original recipient.

This is not an internal development build obtained from a later leak. It was software deliberately distributed outside Blizzard for beta testing.

The historical beta depended on Blizzard-operated infrastructure that no longer exists. The purpose of this preservation work is to make that extinct historical release meaningfully executable again without creating an alternative service for modern StarCraft or modern Battle.net.

---

## Preservation Architecture

The preservation architecture is intentionally separated from the modern retail ecosystem.

```text
1997 StarCraft beta media
          │
          ▼
reconstructed historical client
          │
          │ preservation-specific networking
          ▼
preservation server
```

There is intentionally no compatibility path to the modern retail service:

```text
current retail StarCraft ───X───> preservation server

preservation client ────────X───> current Battle.net
```

The preservation server does **not** recreate the original retail Battle.net network protocol for use by current StarCraft products.

That is deliberate.

The project is intended to preserve a particular extinct historical software environment, not to provide an unauthorized substitute for Blizzard's currently offered network infrastructure.

---

# Historical Beta Test License

The original beta contained a Blizzard **Beta Test License Agreement**.

This section describes issues raised by the text of that historical agreement. It is not a declaration that any particular legal interpretation has already been adjudicated.

The agreement granted the beta tester a license:

> **"for a period not to exceed thirty (30) days"**

Section 3(A) prohibited, among other things, copying, translating, reverse engineering, disassembly, decompilation, and creation of derivative works.

Importantly, Section 3(A) begins:

> **"Subject to the Grant of License above..."**

The stated consequence of violating Section 3(A) was:

> **"the immediate termination of this License Agreement"**

The agreement contains no general clause expressly stating that Section 3(A), including the reverse-engineering restriction, survives expiration of the thirty-day beta-testing relationship indefinitely.

This creates a legitimate contract-interpretation question:

**Was the reverse-engineering prohibition a restriction governing the temporary thirty-day beta relationship, or was it intended to remain an independent perpetual covenant after that relationship ended?**

Nothing in this README claims that expiration of the beta agreement eliminates Blizzard's copyright or itself grants permission to reconstruct copyrighted software.

Copyright, fair use, statutory reverse-engineering provisions, preservation exemptions, interoperability law, and contract law are separate questions.

The narrower point is that the historical agreement should not automatically be described as containing an unquestionably perpetual reverse-engineering prohibition when the agreement itself expressly defined a temporary testing period and did not expressly provide for the survival of Section 3(A).

---

## Express Post-Expiration Language

Section 5 is particularly important when interpreting the agreement because Blizzard expressly described obligations that would arise after the temporary license ended.

It states, in part:

> **"Upon termination of the License Agreement, or upon expiration of the thirty (30) day term..."**

This demonstrates that the agreement knew how to address consequences of expiration expressly.

The absence of comparable language saying that the Section 3(A) reverse-engineering restriction survives expiration is therefore relevant when considering the intended duration of that restriction.

The argument is not that this automatically resolves the question.

The argument is that **the question exists**.

---

# The Destruction Clause

Section 5 also instructed the tester, upon termination or expiration, to:

> **"destroy the Program and all documents and materials you have received"**

and to:

> **"remove any elements of the Program from any hard drives on which the Program has been installed."**

The practical meaning of **"destroy the Program"** is not perfectly precise when applied decades later to surviving original physical distribution media.

The agreement separately refers to:

- the Program;
- copies of the Program;
- documents and materials received;
- and elements installed on hard drives.

It specifically directs removal from hard drives, while not separately stating in plain language:

> "physically destroy the original CD-ROM."

Blizzard could certainly argue that the broader instruction to destroy the Program and materials included destruction of the original physical CD.

The point here is not to pretend that interpretation is impossible.

The point is that the wording and scope of the destruction requirement are **not beyond interpretation**, particularly when attempting to apply a thirty-day beta-testing agreement almost three decades later to a surviving historical artifact.

---

# The Tester Was a Minor

The original recipient of this beta was a minor when the agreement was purportedly accepted.

The recipient did not turn eighteen until **2006**.

That means:

```text
1997
minor receives and installs beta
        ↓
30-day license period
        ↓
Section 5 destruction obligation becomes due
        ↓
recipient is still a minor

2006
recipient reaches adulthood

2026
historical disc still survives
```

Under California law, minors generally can enter contracts, but such contracts are subject to special rules concerning a minor's ability to disaffirm them.

This does **not** mean that the agreement was automatically nonexistent or that a minor can simply ignore copyright law.

It does mean that attempting to treat the 1997 agreement as an ordinary adult commercial contract requires additional legal analysis.

That issue is particularly relevant to the Section 5 destruction requirement because the purported duty to destroy the beta arose while the recipient remained a child.

---

# The Historical Failure to Destroy Is a Separate Issue from Modern Conduct

Another important distinction is between:

1. an alleged contractual breach for failing to destroy the beta when the thirty-day term expired; and
2. conduct occurring decades later involving preservation, reconstruction, or publication.

Those are not necessarily the same legal event.

California ordinarily applies a limitations period to actions based on written contracts.

If Section 5 imposed a one-time duty that became due when the thirty-day beta term expired, a legal question arises as to whether any contractual claim based solely on the historical failure to destroy the disc accrued at that time.

The agreement says:

> **"Upon ... expiration ... you must destroy..."**

It does not expressly say:

> "Possession of the Program shall constitute a new contractual breach every day forever thereafter."

This distinction may matter when determining whether continued possession is:

- a new recurring breach;
- or merely the continuing consequence of an alleged breach that occurred when destruction originally became due.

No claim is made here that a statute of limitations authorizes otherwise unlawful modern conduct.

The point is that **"you failed to destroy the beta in 1997"** and **"you performed preservation work decades later"** should not automatically be collapsed into one legal question.

---

# Does the Reverse-Engineering Clause Survive the Thirty-Day Agreement?

This is one of the central contractual questions surrounding the historical EULA.

The agreement unquestionably prohibited reverse engineering during the beta-testing relationship.

The more difficult question is whether Blizzard drafted that restriction to continue indefinitely after the expressly temporary license expired.

Several features of the text make that question worth asking:

1. The license was expressly limited to no more than **thirty days**.
2. Section 3(A) begins **"Subject to the Grant of License above."**
3. Section 3(A) describes **termination of the License Agreement** as the immediate consequence of prohibited conduct.
4. Section 5 expressly describes obligations triggered by expiration.
5. The agreement contains no general survival clause expressly stating that Section 3(A) survives termination or expiration indefinitely.
6. The person purportedly agreeing to these terms was a minor.
7. The beta-testing relationship and supporting infrastructure have been extinct for decades.

A modern court could still interpret the agreement differently.

The preservation position is simply that the historical agreement should not be casually summarized as:

> "You promised never to reverse engineer this software for the rest of your life."

The actual text is more complicated than that.

---

# Copyright and Preservation Are Separate from the EULA Question

Even if a court concluded that a particular contractual restriction no longer applies, ordinary copyright law would still exist.

Conversely, the fact that copyright remains in force does not automatically answer every question involving:

- fair use;
- reverse engineering;
- interoperability;
- technological circumvention;
- dead-server restoration;
- independent implementation;
- source-level reconstruction;
- archival preservation;
- or publication for historical research.

Software preservation sits at the intersection of several legal doctrines that were developed at different times and for different technological circumstances.

This project does not claim that one thirty-year-old EULA resolves all of those questions.

---

# Dead Infrastructure and Interactive Preservation

A film can generally be preserved by retaining a playable copy of the film.

Interactive software is different.

A game may depend on:

```text
executable
operating system
drivers
graphics API
hardware behavior
copy protection
authentication
matchmaking
network services
timing characteristics
middleware
```

If any essential component disappears, the files may survive while the historical experience does not.

The 1997 StarCraft beta is an example of that problem.

Its original testing environment was temporary by design, and its required online infrastructure disappeared long ago.

Preservation therefore may require more than archival storage.

It can require reconstruction.

---

# Preservation Is Not Commercial Substitution

A central principle of this project is that historical preservation should not become a substitute for an actively sold product or currently operated service.

Accordingly, preservation-specific infrastructure should be technically isolated from modern commercial ecosystems wherever practical.

This project is intended to preserve a specific historical beta experience.

It is **not** intended to:

- provide a replacement modern Battle.net service;
- circumvent authentication for current StarCraft products;
- operate a competing commercial multiplayer network;
- distribute modern Blizzard game content;
- provide access to current Blizzard services without authorization;
- or interfere with Blizzard's present customers.

The deliberate incompatibility between the preservation implementation and modern retail infrastructure is part of the project's preservation philosophy.

---

# Why Historical Builds Deserve Special Attention

A modern remaster and a thirty-year-old beta are not the same historical object simply because they share a franchise name.

A beta may contain:

- different game mechanics;
- different rendering behavior;
- abandoned user interfaces;
- unfinished or removed levels;
- obsolete network architecture;
- different art and sound;
- debugging behavior;
- systems later rewritten entirely;
- and evidence of how the final game evolved.

A current commercial release therefore does not necessarily preserve the historical value of an earlier build.

Preserving the current product and preserving the developmental history of that product are different archival goals.

---

# Long Copyright, Short Technological Lifetimes

Modern copyright terms can last far longer than the technological environments required to experience software.

A software build may have a practical commercial and technological life measured in years while its copyright remains enforceable for many decades.

That creates a preservation problem unique in scale to software:

```text
commercial relevance:     years
hardware/API relevance:   years or decades
copyright duration:       many decades
historical significance:  potentially centuries
```

The preservation position advanced here is not that intellectual property should cease to exist after a few years.

A more sensible distinction is between:

- **the underlying intellectual property**, which may remain strongly protected; and
- **specific historical software builds**, which should eventually receive broader preservation, repair, modding, interoperability, and research protections.

A possible future policy model would provide a finite period of maximum control over a specific build—perhaps five to ten years—after which narrowly defined preservation rights become substantially stronger without placing the underlying franchise, trademarks, characters, art, music, or current products into the public domain.

In short:

> **Long protection for the franchise; shorter maximum exclusivity for the executable build.**

---

# IP Warehousing and Preservation

There is also a broader problem created by consolidation in the software and game industries.

Large corporations can acquire extensive catalogs of intellectual property through mergers and acquisitions.

An acquisition can eliminate an independent rights holder that might otherwise have:

- developed a sequel;
- licensed another developer;
- sold the property to a new studio;
- authorized preservation;
- licensed a remaster;
- released historical source;
- permitted community development;
- or pursued an entirely different commercial strategy.

When many historically important properties become concentrated in a small number of corporate portfolios, those previously independent decisions become centralized.

Some acquired properties may then remain dormant for decades.

The preservation concern is not simply that an owner chooses not to make a new game.

The concern is the combination:

```text
acquire independent IP
        ↓
independent owner disappears
        ↓
historical property is shelved
        ↓
no development
no licensing
no preservation program
        ↓
third-party preservation is also prohibited
        ↓
historical artifact becomes technologically inaccessible
```

Ownership of copyright does not disappear merely because a work is dormant.

At the same time, copyright should not be stretched into a practical mechanism by which accumulated portfolios of abandoned software become impossible for anyone to maintain, study, reconstruct, or experience for generations.

---

# Competition and Warehoused IP

Dormant intellectual property is not necessarily economically irrelevant.

A franchise that is not being exploited today can remain a **potential competitive asset**.

An independent owner could revive it, license it, sell it, authorize a new studio, or use it to compete for developers, investment, customers, and cultural attention.

Serial acquisitions can reduce the number of independent entities capable of making those decisions.

For preservation purposes, this matters because consolidation can also reduce the number of independent organizations deciding what happens to historical software.

This README does not claim that merely owning or declining to use an intellectual property right constitutes an antitrust violation.

Rather, it recognizes that the accumulation and warehousing of large historical software catalogs can have consequences for:

- competition;
- innovation;
- licensing;
- preservation;
- archival access;
- and the survival of culturally important software.

---

# Copyright Misuse and Overbroad Enforcement

Copyright owners have legitimate rights to protect their copyrighted expression.

There is nevertheless an important distinction between:

> enforcing copyright against unauthorized distribution of the copyrighted commercial work

and:

> attempting to use copyright to prohibit an entire independent preservation or compatibility ecosystem surrounding an abandoned historical artifact.

If a rights holder sought only to prevent distribution of identifiable protected expression, that would present one kind of dispute.

If a rights holder instead attempted to prohibit:

- independently authored preservation infrastructure;
- independently implemented compatibility software;
- documentation of functional behavior;
- preservation-specific servers;
- independently reconstructed functionality;
- or any technical mechanism by which an abandoned historical build could remain usable,

then questions concerning the proper scope of copyright enforcement become more significant.

Copyright misuse is a legal doctrine that, in some jurisdictions and factual circumstances, can limit enforcement where copyright is used to obtain control outside the proper scope of the copyright grant.

Nothing here claims that the doctrine necessarily applies to this project.

The broader principle is simply that copyright ownership should protect copyrighted expression without automatically becoming a perpetual veto over the existence of independent historical-preservation technology.

---

# Market Harm

Preservation of an obsolete beta should also be distinguished from ordinary commercial piracy.

A straightforward piracy case may involve:

```text
currently sold product
        ↓
unauthorized competing copy
        ↓
customer buys unauthorized copy instead
        ↓
identifiable lost sale
```

Historical preservation can present a very different situation:

```text
specific historical build not offered for decades
        ↓
original required infrastructure gone
        ↓
preservation reconstruction
        ↓
deliberately incompatible with current product
```

Where a preservation project does not replace a currently sold product or active service, determining actual commercial harm may be substantially more complicated than in an ordinary counterfeit-software case.

That does not mean absence of obvious lost sales automatically makes an activity lawful.

It means that **the factual question of market substitution should be addressed honestly rather than assumed merely because the same company owns both the historical artifact and a modern product bearing the same franchise name**.

---

# Source Reconstruction as Historical Documentation

Source reconstruction necessarily involves interpretation.

During reconstruction, decisions must be made about:

- function boundaries;
- data structures;
- control flow;
- timing behavior;
- state machines;
- network behavior;
- bugs and quirks;
- inferred intent;
- and ambiguous implementation details.

Those decisions are themselves historically useful.

A future researcher may not know which behavior was authentic and which behavior was introduced by a later reconstruction.

Accordingly, preservation projects should clearly distinguish:

```text
confirmed from original artifact
inferred from behavior
reconstructed from analysis
changed for compatibility
changed for preservation
unknown / unresolved
```

That documentation may eventually be as valuable as the resulting executable.

---

# Recommended Archival Structure

A serious preservation project should attempt to preserve more than source code alone.

For example:

```text
/docs
    historical context
    provenance
    known behaviors
    reconstruction methodology
    retail differences
    ambiguous findings
    preservation decisions

/source
    reconstructed client
    preservation server

/tests
    behavioral regression tests
    timing tests
    state tests
    compatibility tests

/build
    toolchain information
    compiler versions
    build instructions

/reference
    hashes of original media
    screenshots
    expected output
    hardware notes
```

Original copyrighted game assets should be handled separately and according to applicable law and project policy.

---

# Provenance

For historical preservation, provenance matters.

Original physical media should be documented where possible through:

- photographs;
- disc labels;
- packaging;
- accompanying documents;
- hashes of archival images;
- dates;
- known distribution history;
- and records describing how the artifact was originally obtained.

This helps distinguish historically distributed software from later unauthorized internal leaks and provides future researchers with confidence that they are studying an authentic artifact.

---

# Preservation Principles

This project follows several basic principles:

1. **Preserve historical behavior, not merely screenshots or binaries.**
2. **Document what is known, what is inferred, and what remains uncertain.**
3. **Avoid substituting for currently sold products or services.**
4. **Keep preservation infrastructure isolated from modern commercial ecosystems where practical.**
5. **Preserve source and methodology so future generations can continue the work.**
6. **Respect the distinction between historical preservation and commercial piracy.**
7. **Retain provenance and validation material for historically significant artifacts.**
8. **Treat software as cultural history whose execution environment may require preservation as much as its files do.**

---

# Legal Notice

This README discusses historical license language, software-preservation policy, and potential legal arguments for informational and documentary purposes.

It is **not legal advice**, does not purport to determine the enforceability of any particular contractual provision, and does not claim that copyright protection has expired.

Questions concerning contract interpretation, copyright, fair use, reverse engineering, interoperability, anti-circumvention law, preservation exemptions, statutes of limitation, and distribution of reconstructed source code are fact-specific and may be unsettled.

Nothing in this document should be interpreted as claiming ownership of Blizzard Entertainment's copyrighted intellectual property, trademarks, characters, artwork, audio, or other protected content.

**StarCraft**, **Blizzard**, and **Battle.net** are trademarks or intellectual property of their respective owners.

This project is an independent historical preservation effort and is not affiliated with, endorsed by, or sponsored by Blizzard Entertainment or Microsoft.

---

## Closing Statement

Software is part of our cultural history.

A surviving disc is not enough if nobody can execute it.

A surviving executable is not enough if nobody can understand or maintain it.

A surviving video is not enough if the interactive work itself has disappeared.

The people who lived through the earliest generations of networked games still possess knowledge that future historians will not.

The purpose of source reconstruction and software preservation is to capture that knowledge while it still exists, preserve the technology required to experience these works, and ensure that future generations inherit something more meaningful than screenshots of software they can never run.

**Preserve the artifact. Preserve the implementation. Preserve the experience.**
