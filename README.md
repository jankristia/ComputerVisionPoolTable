# ComputerVisionPoolTable

Forklaring til deg Simen! Happy reading!
Halla! Jeg tenkte at jeg her forklarer deg litt kode-spesifike ting, typ hvordan kjøre
de forskjellig modulene, hvordan ting henger sammen osv. I tillegg til hva som gjenstår ogsånn.
Hvis du lurer mer på tanken bak det jeg har gjort i modulene tenkte jeg å begynne på rapporten og skrive det der.
Men her skriver jeg hvor jeg tenker tuning kan funke og hva jeg har prøvd

1. Kjøring av koden
   etter cmake kjøres : ./finalProject ../include/Dataset/game1_clip2/game1_clip2.mp4 ../include/Dataset/game1_clip2/bounding_boxes/frame_first_bbox.txt
   i kommandolinjen, det er altså path fra build-mappen til videoen og til groundTruth for deteksjon av baller. Du må kanskje legge til path til maskene
   når du skal gjøre benchmarking av mIoU
   Du får nok heller ikke kjørt koden før du har lastet ned MultiTracker, ref neste punkt  (cliffhanger)

3. MultiTracker

  Første problem du kan få er at jeg har brukt MultiTracker i trackingen av ballene.
  MultiTracker er ikke en standard del når man laster ned openCV, så jeg måtte rote med å laste det ned:
  Hvordan laste det ned?
  - MultiTracker ligger i tillegspakken opencv_contrib
    
  - Først avinstaller alt som har med opencv å gjøre, det trengs for å få brukt riktig versjon av filer når du laster ned opencv (og opencv_contrib) igjen
    Jeg fant en kommando som slettet alle filer og mapper som het noe med opencv, bruk chatten og google, husker den ikke
    
  - Last ned opencv igjen ved å klone både repoet til opencv og opencv_contrib repo
  
  - VIKTIG:: last ned SAMME VERSJON av opencv og opencv_contrib, og ikke last ned nyeste, for den har ikke MultiTracker
    Jeg lastet ned 3.4 tror jeg, og det funket fint. Bare sjekk at MultiTracker finnes i den versjonen av opencv_contrib du går for.
  Du kan kanskje finne en annen måte å tracke på så du slipper nedlastningen, men virket som MultiTracker var vanlig å bruke(fra googling)

3. Bruk av de forskjellige klassene og hvor bra de funker:

   I alle h-filene har jeg kommentert kort hvilken funksjon som typ gjør "klassens jobb"
   For at du skal slippe å sette deg for mye inn i hver klasse har jeg gjort sånn at du må initialisere en tom instans av en klasse også kjøre en funksjon
   Så gjør denne funksjonen hele klassens jobb.
   
  3.1 TableDetector
   
   detectTable(frame); tar inn en frame, detekterer bordet, setter alt rundt til svart og både setter roiTable i klassen, samt returnerer det samme
   Table detector funker bra!
   
  3.2 BallDetector
   
   ballDetector.segmentBalls(tableDetector.roiTable); Denne funksjonen skal ta inn et bilde av et segmentert bord og den detekterer ballene og setter da
   vektoren detectedBalls, samt at den setter vektoren segmentedBalls. segmentedBalls er det vi bruker mest utenfor klassen da det er en vektor av en struct med
   posisjon, størrelse og typen av ballen.
   Deteksjonen av ballene er gjort med Hough(Dick)Transform, og da hele sullamitten av pre-prossesering før(blurring, equalization, Canny). Det funket overraskende bra å kjøre
   stor bulrring. Jeg har gjort så vi detekterer litt for mange baller også fjerner jeg etterpå de med lik farge som bordet.
   Deteksjonen funker ok-- nå, kan nok tunes, da i void detectBalls(cv::Mat frame);
   Segmenteringen av ballene funker bra for hvit og sort ball når de er detektert riktig, men ganske ræva for striper/hel-kule. Tanken der er at jeg først detekterer hvit/sort kule
   med at de har flest hvite/sorte piksler også fjerner jeg de fra vektoren og ser på alle bboksene og teller antall hvite pixles mange hvite da --> stripete ball
   Segmenteringen bør nok tunes litt, kan gjøres ved å sette nytt threshold for antall hvite som fører til stripete ball  
 
  3.3 BallTracker
  
   trackBalls(cv::VideoCapture video); er funksjonen som gjelder her. Den er lagd med såpass dårlig kodekvalitet at den bare tar inn en video, også
   bruker den TableDetector og BallDetector inni seg til å detektere bord, baller, bboxer og prøve å tracke ballene gjennom videoen.
   Tanken bak avhengigheten er at det skal gå fort for deg å tune ball-deteksjon-funksjonen også kjøre trackingen og se hvordan det funker.
   Altså for å kjøre trackingen, trenger du bare en video som kommer til å være i main når jeg pusher siste, også kjøre disse linjene:
   // BallTracker ballTracker;
   // ballTracker.trackBalls(video);
   Trackingen funker forøvrig dårlig, men jeg har fått best resultater av å utvide bboksene fordi når de slår ballen går den så fort at den går ut av
   boksen mellom frames ved bbokser som er close rundt ballen. TrackerBoosting:: er den som har funket best til nå, men du kan prøve deg litt frem, for per nå funker det dårlig
   
  3.4 MeanAveragePrecision
  
   Dette er benchmarkingen for deteksjon av baller. sånn jeg tolket det så skal vi benchmarke kun deteksjonen av ball/ikke ball her
   averagePrecisionCalculation(cv::Mat frame, std::string groundTruthPath); gjør alt av deteksjon og regning av MAP så det er bare å initialisere et objekt
   og kjøre denne. Det skal i rapporten gjøres for både første og siste bilde for alle videoen (de bildene ligger i datasettet, så må ikke via videoen)
   Jeg tenkte du kunne legge det inn når du har tunet ferdig:)

  3.5 HelperFunctions

    Det som ligger der per nå er egt bare funksjoner for å trykke på bildet og printe pixel-verdier. Det er sånn jeg har 
    funnet thresholds for hvit og svart.

4. Hva gjenstår?

   As far as jeg veit så er det som gjenstår:
   - lage top-down view som vises i videoen mens ballene trackes
   - Lage UiO benchmarking av segmenteringen av bord/bakgrunn/ball-typene
       - Her er vel det meste av segmenteringen gjort, den må bare settes i system og testes
    - Tuning til det funker så bra du kan få det (det blir aldri bra)
    - Få inn resultater og skrive ferdig rapporten når du er lei av tuning
  
      Jeg kommer til å begynne på rapporten, og:
      - Skrive ferdig delen om bordet
      - Forklaring på hvorfor jeg har gjort ting i de forskjellige klassene
      Men resultatene kan du ta når du har tunet bra, evt kommet på smartere løsninger:)

Lykke til buddy
