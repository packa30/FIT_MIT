-- Project : simplify-bkg
-- Author  : Peter Grofcik
-- Login   : xgrofc00
-- Year    : 2020
-- Algoritmus 4.2 - deletion of unnecessary symbols
module Second where
  import Data.List.Split
  import Syntax
  import First

  -- function that creates new grammar from obtained BKG1 set
  -- uses obtainVi to obtain Vi set that stands for available symbols
  -- afterwars uses obtainNsig to obtain new n & sig set with only available symbols
  -- also uses ontainPK function from module First.hs to select rules that consts only of set Vi symbols
  -- creates empty grammar if it was flagged as one in stage 1
  obtainBKG2 :: BKG -> BKG
  obtainBKG2 bkg = do
    let vi = obtainVi [s bkg] bkg
    if syntax bkg then
      BKG False (obtainNsig (concat vi) (n bkg) []) (obtainNsig (concat vi) (sig bkg) []) (s bkg) (obtainPK vi [] (p bkg))
    else
      BKG False [s bkg] [""] (s bkg) [s bkg ++ "->#"]

  -- function to obtain Vi set that stands for available symbols
  -- recursive call until obtained Vi set gets changed in process
  obtainVi :: [String] -> BKG -> [String]
  obtainVi s bkg = do
    let s1 = obtainVi1 s (p bkg)
    if s1 /= s then
      obtainVi s1 bkg
    else
      s1

  -- function to obtain "actual" Vi set that stands for available on single index
  -- it simply checks all the rules left sides if they are in Vi set from index-1 trough reqursive call
  -- but if it is it adds symbols from the right side of the rule to the actual Vi set with a help call of obtainVila
  obtainVi1 :: [String] -> [String] -> [String]
  obtainVi1 s [] = s
  obtainVi1 s p =
    if check (concat s) (head (splitOn "->" (head p))) &&
      not(check (concat s) (splitOn "->" (head p)!!1)) then
      obtainVi1 ( obtainVila s (map (:[])(splitOn "->" (head p)!!1))  )  (tail p)
    else
      obtainVi1 s (tail p)

  -- function that goes trough all the symbols form the right side of the rull
  -- and adds them to a Vi set of actuial index if they are not already present
  obtainVila s [] = s
  obtainVila s str =
    if not(check (concat s) (head str) ) then
      obtainVila (s ++ [head str]) (tail str)
    else
      obtainVila s (tail str)

  -- function that goes trough symbols and finds if they are present in Vi set
  -- based on that it creates new symbol set adding up all the symbols that are
  -- used to obtain new n & sig sets from complete Vi set
  obtainNsig :: (Foldable t, Eq a) => t a -> [[a]] -> [[a]] -> [[a]]
  obtainNsig vi [] nn = nn
  obtainNsig vi n [] =
    if check vi (head n) then
      obtainNsig vi (tail n) [head n]
    else
      obtainNsig vi (tail n) []

  obtainNsig vi n nn =
    if check vi (head n) then
      obtainNsig vi (tail n) (nn ++ [head n])
    else
      obtainNsig vi (tail n) nn
