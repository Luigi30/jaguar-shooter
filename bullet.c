#include "bullet.h"

BulletNode *list_Bullets;

void Bullets_Draw(Bullet **head, uint8_t *buffer)
{
  Bullet *current = *head;
  
  while(current != NULL) {
    GPU_do_blit_sprite(buffer, current->location, shipsheet, SPRITES_find("PulseBullet1"));

    current = current->next;
  }
  
}

//
BulletNode *BulletNode_Create(Coordinate _location, SpriteGraphic *_image, Coordinate _deltaPerFrame, bool _isPlayer)
{
  BulletNode *bullet = calloc(1, sizeof(BulletNode));

  bullet->location = (Coordinate){ .x = _location.x, .y = _location.y };
  bullet->image = (SpriteGraphic){ .location = _image->location, .size = _image->size };
  bullet->deltaPerFrame = (Coordinate){ .x = _deltaPerFrame.x, .y = _deltaPerFrame.y };
  bullet->isPlayer = _isPlayer;

  strcpy(bullet->image.name, _image->name);

  return bullet;
}

void BulletsList_Update(struct List *bullets)
{
  for(BulletNode *current = bullets->lh_Head; current->node.mln_Succ; current = current->node.mln_Succ)
    {
      //Abort if list is empty.
      if(bullets->lh_Head == bullets->lh_TailPred)
	break;

      //struct Node *prev = current->node.mln_Pred;
      //struct Node *next = current->node.mln_Succ;
      //sprintf(skunkoutput, "bullet: addr %08p, location (%03d,%03d). pred is %08X succ is %08X\n", current, current->location.x, current->location.y, prev, next);
      //skunkCONSOLEWRITE(skunkoutput);
      
      current->location.x += current->deltaPerFrame.x;
      current->location.y += current->deltaPerFrame.y;
      if(current->location.y < 5)
	{	  
	  struct Node *temp = current;
	  current = current->node.mln_Succ;
 	  Remove(temp);
	  free(temp);

	  if(current == NULL || (long)current > 0xFFFFFF)
	    break;
	}
    }
}

void BulletsList_Draw(struct List *bullets, uint8_t *buffer)
{  
  for (BulletNode *current = (SpriteNode *)bullets->lh_Head; current->node.mln_Succ; current = current->node.mln_Succ)
    {    
      GPU_do_blit_sprite(buffer, current->location, shipsheet, SPRITES_find(current->image.name));
    }
}
