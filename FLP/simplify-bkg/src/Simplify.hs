-- Project : simplify-bkg
-- Author  : Peter Grofcik
-- Login   : xgrofc00
-- Year    : 2020

import System.Environment
import Data.List
import Data.List.Split
import Syntax
import First
import Second

main :: IO ()
main = do
  args <- getArgs
  let (arg1,arg2) = arguments args
  contents <- if null arg2 then
                getContents
              else
                readFile arg2
  let bkg = checkSyn contents

  if not(syntax bkg) then
    print "Wrong Syntax"
  else if arg1 == "-i" then
    printX bkg
  else if arg1 == "-1" then
    printX (obtainBKG1 bkg)
  else if arg1 == "-2" then
    printX (obtainBKG2 (obtainBKG1 bkg))
  else
    print "First argument is wrong -> choose one of [ -i , -1 , -2 ]"

arguments :: [String] -> (String, String)
arguments [x] = (x,"")
arguments [x,y] = (x,y)
