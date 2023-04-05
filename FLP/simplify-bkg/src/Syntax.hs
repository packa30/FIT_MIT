-- Project : simplify-bkg
-- Author  : Peter Grofcik
-- Login   : xgrofc00
-- Year    : 2020
module Syntax where
  import Data.List.Split

  -- data type (structure) intern representation of loaded, later changed BKG
  data BKG = BKG{ syntax :: Bool,
                  n      :: [String],
                  sig    :: [String],
                  s      :: String,
                  p      :: [String] }

  -- obtain first 3 indexes from [] -> used to obtain N sig S (first three rows of inserted BKG)
  getcon :: [a] -> (a, a, a)
  getcon x = (head x,x!!1,x!!2)

  --check inserted string ([char]) against occurence in desired [char]
  -- -> used for syntax check of N sig S, but also desired side of P (rule) against N,sig or S
  check :: (Foldable t, Eq a) => t a -> [a] -> Bool
  check a [] = True
  check a xs = elem (head xs) a && check a (tail xs)

  --function that checks each rule of BKG for syntax using recursive transition trough a [[char]] of rules
  -- -> uses check for each side of a single rule
  checkP :: String -> String -> [String] -> Bool
  checkP n sig [] = True
  checkP n sig p = do
    let rule = splitOn "->" (head p)
    if check n (head rule) && -- check for left side of a rule
       ( check (n ++ sig) (concatMap (:[]) (rule!!1)) || ( (rule!!1) == "#") ) && --check for right side of a rule
       ( head rule /= (rule!!1) ) && -- check against eq in sides of rule -- A->A should not occur
       ( head rule /= "" && (rule!!1) /= "" ) &&  --check if either side of a rule is not empty
       checkP n sig (drop 1 p) then
      True
    else
      False

  --destruction of "" in a last position if it exists -> created by split on \n
  lastRule :: [String] -> [String]
  lastRule p =
    if last p == "" then
      init p
    else
      p

  --main function for syntax check
  -- -> splits loaded content on \n
  -- -> uses getcon to obtain first 3 rows as N sig S
  -- -> the rest is considered as rules p
  -- -> calls check && checkP to confirm correct syntax in loaded bkg
  -- -> returns BKG data type with True/False depending on syntax verification
  -- -> # is added to inside representation of sig in BKG
  --    -> saves multiple if conditions later, but ignored in output of sig
  checkSyn :: String -> BKG
  checkSyn contents = do
    let incon = splitOn "\n" contents
    let (n,sig,s) = getcon incon
    let p = lastRule (drop 3 incon)
    if check ['A' .. 'Z'] (concat(splitOn "," n)) &&
       check ['a' .. 'z'] (concat(splitOn "," sig)) &&
       check (concat(splitOn "," n)) s &&
       checkP (concat(splitOn "," n)) (concat(splitOn "," sig)) p then
      BKG True (splitOn "," n) (splitOn "," sig ++ ["#"]) s p
    else
      BKG False (splitOn "," n) (splitOn "," sig ++ ["#"]) s p

  --output function for BKG representation
  printX :: BKG -> IO()
  printX bkg = do
    putStr (head (n bkg))
    printNTS (drop 1 (n bkg))
    putStr (head (sig bkg))
    printNTS (drop 1 (sig bkg))
    putStr (s bkg)
    putStr "\n"
    printP (p bkg)

  --used for N & sig output
  printNTS :: [String] -> IO ()
  printNTS ["#"] = putStr "\n"
  printNTS [] = putStr "\n"
  printNTS x = do
    putStr ","
    putStr (head x)
    printNTS (drop 1 x)

  --used for correct output of P (rules)
  printP :: [String] -> IO ()
  printP [] = return()
  printP x = do
    putStrLn (head x)
    printP (drop 1 x)
