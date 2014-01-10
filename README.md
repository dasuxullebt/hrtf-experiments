hrtf-experiments
================

This is a small part of my personal experiments with several
HRTF-Databases, currently mainly the
[MIT-HRTF-Measurements](http://sound.media.mit.edu/resources/KEMAR.html). I
am in no way affiliated with the MIT Media Lab, nor am I a
professional in this topic. I hope that this code will be useful for
others to understand how to use the MIT Database, and maybe I can get
feedback from other people.

The algorithm used for applying the finite impulse responses is simple
but slow, but sufficient for my purposes. If you need something
better, you should probably consider using, for example, the
[Overlap–save
method](https://en.wikipedia.org/wiki/Overlap%E2%80%93save_method). Also
consider the code examples provided by the MIT with their database.

Dependencies
------------

The code is written under and for Linux, but should be easy to
port. For the code to work, you need the MIT HRTF Database. Use

    ./download.sh

for downloading and unzipping the database. It will create a directory
"compact". Notice that this part is **not** my work, and not covered
by the license of this code.

To compile the library, use

    gcc -c -g -o hrtf.o hrtf.c
    gcc -g -o example example.c hrtf.o -lm

Now, use stereo headphones, and play around with it, for example with

    yes 123456789012345678901234567890 | ./example -30 -90 | aplay -f U8 -c2 -r44100
    yes 123456789012345678901234567890 | ./example -30 90 | aplay -f U8 -c2 -r44100
    yes 123456789012345678901234567890 | ./example 0 0 | aplay -f U8 -c2 -r44100

Hope this works for you.

Reasons for GPLv3
-----------------

I chose the GPLv3 as a license for this project. Let me briefly
explain why.

There are a lot of closed or semi-closed libraries around there, which
is, in my opinion, blocking progress. I am crabby. Even though my code
does not do much, if I released it to the public domain, there is a
slight chance that it will make the work of some commercial programmer
easier, and spare a few cents for some company that does not do
anything valuable for society and is mainly interested in personal
profit. (Of course, this does clearly not hold for every company.)

I have no commercial interests related to this project. If you, for
some reason, need my code in some of your projects, but cannot comply
with the GPLv3 for some reason, feel free to contact me. If it makes
the world better, I will probably give you a liberal license for your
project.

Contact
-------

Visit my website

	(uxul.de)[https://uxul.de]

Send questions, suggestions, lorazepam and kappa maki to

    printf("%s@%s.de\n", "css", "uxul");
