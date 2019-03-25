// Author:

#include "pixel.h"
#include "photo.h"

//definition du foncteur servant a trier nos objets selon un critere
struct MySort{
    bool operator ()(int* a,int* b) const{
        return *a<*b;
    }
};

int main(int argc, char* argv[]) {
    Image<byte> I;
    if(! load(I, argc>1? argv[1]: srcPath("salon.png"))) {
        std::cout << "Echec de lecture d'image" << std::endl;
        return 1;
    }

    //on cree la "photo" associee a l'image
    Photo P=Photo(I.height(),I.width());

    for(int i=0;i<P.get_n();i++)
        for(int j=0;j<P.get_m();j++){
            P.set(i,j);
        }

    //connexite
    //premier parcours : nb d'etiquettes   max
    int nb_etiquettes=0;
    std::vector<Etiquette> E; // vecteur des etiquettes utilises
    std::vector<int*> pointeurs_etq; // vecteur des pointeurs attribues a chaque valeur des etiquettes utilises
    for(int i=0;i<P.get_n();i++){
        for(int j=0;j<P.get_m();j++){
            Pixel p=P.get(i,j); ////ca me permet de modifier et garder l'info pour le Pixell?
            if(p.nb_etiquettes_vois==0){
                //creer un nouveau pointeur pour la nouvelle etiquette
                int* pointeur= new int(nb_etiquettes+1);
                //update le pointeur du pixel
                p.etiquette=pointeur;
                //rajouter le pointeur au vecteur
                pointeurs_etq.push_back(pointeur);
                //rajouter etiquette voisine aux voisins
                P.update_voisins(i,j,p.etiquette);
                //incrementer le nombre d'etiqiettes uttilisees
                nb_etiquettes+=1;
                //rajouter l'etiquette au vecteur
                Etiquette e=Etiquette(*(p.etiquette));
                E.push_back(e);
            }
            if(p.nb_etiquettes_vois==1){ //alors etiquettes_vois ne contient que une seule valeur (peut etre plusieurs fois)
                //update le pointeur du pixel: il prend la valeur de l'etiquette du pixel voisin
                p.etiquette=p.etiquettes_vois[0];
                //incrementer la taille de l'etiquette
                for(int k=0;k<E.size();k++){
                    if(E[k].numero==*(p.etiquette)){
                        E[k].taille+=1;
                    }
                }
                //rajouter etiquette voisine aux voisins
                P.update_voisins(i,j,p.etiquette);
            }
            if(p.nb_etiquettes_vois>1){
                //update du pointeur du pixel: il prend la valeur de l'etiquette voisine de valeure minimale
                p.etiquette=*(std::min_element(p.etiquettes_vois.begin(),p.etiquettes_vois.end(),MySort()));
                //incrementer la taille de l'etiquette
                for(int k=0;k<E.size();k++){
                    if(E[k].numero==*(p.etiquette)){
                        E[k].taille+=1;
                    }
                }
                //rajouter etiquette voisine aux voisins
                P.update_voisins(i,j,p.etiquette);
            }
        }
    }
    // deuxieme parcous pour rassembler les etiquettes qui se touchent
    for(int i=0;i<P.get_n();i++){
        for(int j=0;j<P.get_m();j++){
            Pixel p=P.get(i,j);
            //si le Pixel considere a des voisins avec une etiquette ...
            if(p.nb_etiquettes_vois>0){
                std::vector<int*>::iterator it;
                for(it=p.etiquettes_vois.begin();it<p.etiquettes_vois.end();it++){
                    //... et qu'il existe un voisin avec une etiquette plus petite
                    if(*(*(it))<*(p.etiquette)){
                        //on trouve le pointeur initial qui correspond a la valeur de l'etiquette du pixel considere
                        int* etq_a_changer=*(std::find(pointeurs_etq.begin(),pointeurs_etq.end(),p.etiquette));
                        //fusionnement des deux etiquettes:
                        //on change sa valeur ce qui devrait changer la  valeur des pointeurs etiquettes de tous les pixels appartenants a cette etiquette
                        etq_a_changer=*(it);
                        //le nbb d'etiquettes utilises diminue
                        nb_etiquettes-=1;
                    }
                }
            }
        }
    }
    //si une etiquette touche l'autre on les met a la meme valeur (la plus petite)
    // on ne regarde que les pixels du bord
    //nb_etiquettes doit correspondre a des etiquettes differentes
    //pointeur pour chaque etiquette (et un pointeur par pixel qu on met a jour )

   return 0;
}
