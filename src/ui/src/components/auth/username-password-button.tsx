/*
 * Copyright 2018- The Pixie Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This is the primary auth box, which has either the login or signin variant.
import * as React from 'react';
import { Button } from '@mui/material';
import { Theme } from '@mui/material/styles';
import { createStyles, makeStyles } from '@mui/styles';

const useStyles = makeStyles(({ spacing }: Theme) => createStyles({
  button: {
    paddingTop: spacing(1),
    paddingBottom: spacing(1),
    textTransform: 'capitalize',
  },
}));

export interface UsernamePasswordButtonProps {
  text: string;
  onClick: () => void;
}
export const UsernamePasswordButton = React.memo<UsernamePasswordButtonProps>(function UsernamePasswordButton({
  text,
  onClick,
}) {
  const classes = useStyles();
  return (
    <Button
      variant='contained'
      color='primary'
      className={classes.button}
      onClick={onClick}
    >
      {text}
    </Button>
  );
});
