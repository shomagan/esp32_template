@echo offREM Run generator first
cd generator
python regs_generator.py
cd ..

REM Then build
idf.py build %*
