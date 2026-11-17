/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreRBTree which are only used by the implementation.
 */

/*
 * Copyright (C) 2010-2012 Gedare Bloom.
 * Copyright (C) 2002 Niels Provos <provos@citi.umich.edu>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * The red-black tree code of this file derives from the macros of the
 * FreeBSD <sys/tree.h> file of 2020.
 */

#ifndef _RTEMS_SCORE_RBTREEIMPL_H
#define _RTEMS_SCORE_RBTREEIMPL_H

#include <rtems/score/rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreRBTree
 *
 * @{
 */

/**
 * @brief Appends the node to the red-black tree.
 *
 * The appended node is the new maximum node of the tree.  The caller shall
 * ensure that the appended node is indeed the maximum node with respect to the
 * tree order.
 *
 * @param[in, out] the_rbtree is the red-black tree control.
 *
 * @param the_node[out] is the node to append.
 */
void _RBTree_Append( RBTree_Control *the_rbtree, RBTree_Node *the_node );

/**
 * @brief Prepends the node to the red-black tree.
 *
 * The prepended node is the new minimum node of the tree.  The caller shall
 * ensure that the prepended node is indeed the minimum node with respect to the
 * tree order.
 *
 * @param[in, out] the_rbtree is the red-black tree control.
 *
 * @param the_node[out] is the node to prepend.
 */
void _RBTree_Prepend( RBTree_Control *the_rbtree, RBTree_Node *the_node );

/**
 * @brief Red-black tree visitor.
 *
 * @param[in] node The node.
 * @param[in] visitor_arg The visitor argument.
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 *
 * @see _RBTree_Iterate().
 */
typedef bool ( *RBTree_Visitor )( const RBTree_Node *node, void *visitor_arg );

/**
 * @brief Red-black tree iteration.
 *
 * @param rbtree The red-black tree.
 * @param visitor The visitor.
 * @param visitor_arg The visitor argument.
 */
void _RBTree_Iterate(
  const RBTree_Control *rbtree,
  RBTree_Visitor        visitor,
  void                 *visitor_arg
);

/**
 * @brief Checks if the node is red.
 *
 * @param the_node is the node or NULL.
 *
 * @return Returns true, if the node is not NULL and red, otherwise false.
 */
static inline bool _RBTree_Is_red( const RBTree_Node *the_node )
{
  return the_node != NULL && _RBTree_Color( the_node ) == RTEMS_RB_RED;
}

/**
 * @brief Returns the parent of the node of the specified color.
 *
 * @param the_node is the node.
 *
 * @param color is the color of the node.
 *
 * @return Returns the parent of the node.
 *
 * @par Notes
 * Bit 1 of the parent and color word of a node in a tree is zero.  The
 * function subtracts the color from the word to get the parent.  The compiler
 * folds the subtraction of a constant color into the offset of the next load
 * through the parent.
 */
static inline RBTree_Node *_RBTree_Parent_of_color(
  const RBTree_Node *the_node,
  int                color
)
{
  _Assert( _RBTree_Color( the_node ) == color );
  return (RBTree_Node *) ( the_node->rbn_parent_color - (uintptr_t) color );
}

/**
 * @brief Returns the parent and color word of the node.
 *
 * @param the_node is the node.
 *
 * @return Returns the parent and color word of the node.
 */
static inline uintptr_t _RBTree_Get_parent_color( const RBTree_Node *the_node )
{
  return the_node->rbn_parent_color;
}

/**
 * @brief Sets the parent and color word of the node.
 *
 * @param[out] the_node is the node.
 *
 * @param parent_color is the parent and color word.
 */
static inline void _RBTree_Set_parent_color_word(
  RBTree_Node *the_node,
  uintptr_t    parent_color
)
{
  the_node->rbn_parent_color = parent_color;
}

/**
 * @brief Replaces the old node by the new node in the link of the parent.
 *
 * @param[in, out] head is the tree.
 *
 * @param old_node is the old node.
 *
 * @param[in, out] new_node is the new node.
 *
 * @param[in, out] parent is the parent of the old node, else NULL.
 */
static inline void _RBTree_Relink_child(
  RBTree_Control *head,
  RBTree_Node    *old_node,
  RBTree_Node    *new_node,
  RBTree_Node    *parent
)
{
  if ( parent == NULL ) {
    _RBTree_Set_root( head, new_node );
  } else if ( _RBTree_Left( parent ) == old_node ) {
    _RBTree_Set_left( parent, new_node );
  } else {
    _RBTree_Set_right( parent, new_node );
  }
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
