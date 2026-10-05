# tests/corpus/alyan/ -- corpora Alyan's tests and accuracy runs use

Generate, do not commit:

    python3 tools/alyan_gen_corpus.py --out tests/corpus/alyan/medium         --files 1000 --seed 7 --plagiarism 0.2

and the 1 GB stress corpus:

    python3 tools/alyan_gen_corpus.py --out /tmp/big --files 10000 --seed 11

Owner: ALYAN. Anas indexes whatever lands here but does not add files.
